#include "Renderpch.h"
#include "VulkanRenderContext.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanGraphicsFence.h"
#include "VulkanQueue.h"
#include "VulkanImage.h"
#include "VulkanImageSampler.h"
#include "../../../Pipeline/CompiledRenderPipeline.h"
#include "../../RenderView.h"
#include "VulkanUtils.h"
#include "../../../RenderService.h"
#include "../../../Mesh.h"
#include "../../../Material.h"
#include "Pools/VulkanDescriptorSetPool.h"

#include <Coco/Core/Math/Random.h>

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanGlobalState::VulkanGlobalState(SharedRef<Shader> shader, VulkanShader& vulkanShader, VulkanPipeline& pipeline) :
		BoundShader(shader),
		BoundVulkanShader(vulkanShader),
		BoundPipeline(pipeline),
		GlobalShaderSet(nullptr)
	{}

	VulkanInstanceState::VulkanInstanceState(uint64 instanceID) :
		InstanceID(instanceID),
		InstanceSet(nullptr)
	{}

	VulkanRenderContextOperation::VulkanRenderContextOperation(
		const CompiledRenderPipeline& pipeline, 
		RenderView& view,
		VulkanCommandBuffer& commandBuffer,
		VulkanRenderPass& renderPass,
		VulkanRenderFrame& renderFrame) :
		Pipeline(pipeline),
		View(view),
		CommandBuffer(commandBuffer),
		CurrentPassIndex(0),
		RenderPass(renderPass),
		RenderFrame(renderFrame),
		GlobalSet(nullptr)
	{}

	const double VulkanRenderContext::_stalePoolThreshold = 2.0;
	const uint64 VulkanRenderContext::_globalDataID = 0;

	VulkanRenderContext::VulkanRenderContext(const GraphicsResourceID& id, VulkanGraphicsDevice& device) :
		RenderContext(id),
		_device(device),
		_renderOperation(),
		_renderCompletedFence(CreateManagedRef<VulkanGraphicsFence>(id, device, true)),
		_globalState(),
		_drawUniforms(),
		_renderStats{}
	{
		CreateCommandBuffer();

		CocoTrace("Created VulkanRenderContext {}", ID)
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		WaitForRenderToComplete();

		_renderCompletedFence.Invalidate();
		_descriptorPools.clear();

		FreeCommandBuffer();

		CocoTrace("Destroyed VulkanRenderContext {}", ID)
	}

	void VulkanRenderContext::SetViewportRect(const RectInt& viewportRect)
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		SizeInt s = viewportRect.GetSize();

		VkViewport viewport{};
		viewport.x = static_cast<float>(viewportRect.GetLeft());
		viewport.y = static_cast<float>(viewportRect.GetBottom());
		viewport.width = static_cast<float>(s.Width);
		viewport.height = static_cast<float>(s.Height);
		viewport.minDepth = 0.0f; // TODO: configurable min/max depth
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(_renderOperation->CommandBuffer.GetCmdBuffer(), 0, 1, &viewport);
	}

	void VulkanRenderContext::SetScissorRect(const RectInt& scissorRect)
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		SizeInt s = scissorRect.GetSize();

		VkRect2D scissor{};
		scissor.offset.x = static_cast<uint32_t>(scissorRect.GetLeft());
		scissor.offset.y = static_cast<uint32_t>(scissorRect.GetBottom());
		scissor.extent.width = static_cast<uint32_t>(s.Width);
		scissor.extent.height = static_cast<uint32_t>(s.Height);

		vkCmdSetScissor(_renderOperation->CommandBuffer.GetCmdBuffer(), 0, 1, &scissor);
	}

	void VulkanRenderContext::SetShader(const SharedRef<Shader>& shader)
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		if (_globalState.has_value() && _globalState->BoundShader == shader)
			return;

		SharedRef<Shader> shaderToUse = shader ? shader : RenderService::Get()->GetErrorShader();

		CocoAssert(shaderToUse, "No shader was bound")

		_instanceState.reset();
		_drawUniforms.clear();

		VulkanGraphicsDeviceCache& deviceCache = _device.GetCache();
		VulkanDescriptorSetLayout& globalSetLayout = deviceCache.GetOrCreateDescriptorSetLayout(_renderOperation->View.GetGlobalUniformLayout(), true);
		VulkanShader& vulkanShader = deviceCache.GetOrCreateShader(shaderToUse);

		VulkanPipeline& pipeline = deviceCache.GetOrCreatePipeline(
			_renderOperation->RenderPass,
			vulkanShader,
			_renderOperation->CurrentPassIndex,
			globalSetLayout
		);

		_globalState.emplace(shaderToUse, vulkanShader, pipeline);

		vkCmdBindPipeline(_renderOperation->CommandBuffer.GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipeline());

		_globalState->GlobalSet = GetOrCreateDescriptorSet(_globalDataID, UniformScope::Global, globalSetLayout);
		BindDescriptorSet(_globalState->GlobalSet, 0);

		VulkanDescriptorSetLayout& shaderGlobalSetLayout = deviceCache.GetOrCreateDescriptorSetLayout(vulkanShader.GetUniformLayout(UniformScope::ShaderGlobal), true);
		_globalState->GlobalShaderSet = GetOrCreateDescriptorSet(vulkanShader.ID, UniformScope::ShaderGlobal, shaderGlobalSetLayout);
		BindDescriptorSet(_globalState->GlobalShaderSet, 1);
	}

	void VulkanRenderContext::Draw(const SharedRef<Mesh>& mesh, const Submesh& submesh)
	{
		if (!mesh)
		{
			CocoError("Mesh was empty. Skipping...")
			return;
		}

		DrawIndexed(mesh, submesh.IndexOffset, submesh.IndexCount);
	}

	void VulkanRenderContext::DrawIndexed(const SharedRef<Mesh>& mesh, uint64 indexOffset, uint64 indexCount)
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		if (!mesh)
		{
			CocoError("Mesh was empty. Skipping...")
			return;
		}

		// Don't bother rendering empty meshes
		if (mesh->GetVertexCount() == 0 && mesh->GetIndexCount() == 0 || indexCount == 0)
			return;

		// Get the cached mesh data
		const CachedVulkanMesh* cachedMesh = nullptr;
		if (!_renderOperation->RenderFrame.TryGetCachedMesh(*mesh, cachedMesh))
		{
			CocoError("Failed to get data for mesh. Skipping...")
			return;
		}

		if (!FlushStateChanges())
		{
			return;
		}

		VkCommandBuffer cmdBuffer = _renderOperation->CommandBuffer.GetCmdBuffer();

		uint64 vertexPositionSize = cachedMesh->VertexCount * GetBufferDataTypeSize(BufferDataType::Float3);
		bool hasOptionalAttributes = mesh->GetVertexFormat().AdditionalAttributes != VertexAttrFlags::None;
		VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(cachedMesh->Buffer.Get());

		// Bind the vertex buffer
		std::array<VkDeviceSize, 2> offsets = { cachedMesh->AllocBlock->Offset, cachedMesh->AllocBlock->Offset + vertexPositionSize };
		std::array<VkBuffer, 2> vertexBuffers = { vulkanBuffer->GetBuffer(), vulkanBuffer->GetBuffer() };
		vkCmdBindVertexBuffers(cmdBuffer, 0, hasOptionalAttributes ? 2 : 1, vertexBuffers.data(), offsets.data());

		// Bind the index buffer
		vkCmdBindIndexBuffer(cmdBuffer, vulkanBuffer->GetBuffer(), cachedMesh->AllocBlock->Offset + cachedMesh->IndexBufferOffset, VK_INDEX_TYPE_UINT32);
		
		// Draw the mesh
		vkCmdDrawIndexed(cmdBuffer,
			static_cast<uint32>(indexCount),
			1,
			static_cast<uint32>(indexOffset),
			0,
			0);

		_renderStats.DrawCalls++;
		_renderStats.TrianglesDrawn += indexCount / 3;
		_renderStats.VerticesDrawn += cachedMesh->VertexCount;
	}

	void VulkanRenderContext::WaitForRenderToComplete()
	{
		if(!IsIdle())
			_renderCompletedFence->WaitForSignal(Math::MaxValue<uint64>());
	}

	bool VulkanRenderContext::IsIdle() const
	{
		return !_renderOperation.has_value() && _renderCompletedFence->IsSignaled();
	}

	void VulkanRenderContext::SetGlobalUniforms(std::span<const ShaderUniformValue> uniforms)
	{
		SetUniforms(_globalDataID, 0, UniformScope::Global, uniforms);
	}

	void VulkanRenderContext::SetGlobalShaderUniforms(std::span<const ShaderUniformValue> uniforms)
	{
		SetUniforms(0, 0, UniformScope::ShaderGlobal, uniforms);
	}

	void VulkanRenderContext::SetMaterial(const SharedRef<Material>& material)
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")
		CocoAssert(_globalState.has_value(), "No shader was bound")

		if (!material)
		{
			_instanceState.reset();
			return;
		}

		uint64 instanceID = material->GetID();
		uint64 dataID = Math::CombineHashes(_globalState->BoundShader->GetID(), instanceID);
		if (_instanceState.has_value() && _instanceState->InstanceID == dataID)
			return;

		_instanceState.emplace(dataID);

		VulkanUniformCache& uniformCache = _renderOperation->RenderFrame.GetUniformCache();

		uint64 instanceVersion = material->GetVersion();

		if (uniformCache.DataNeedsUpdate(dataID, UniformScope::Instance, instanceVersion))
		{
			SetUniforms(dataID, instanceVersion, UniformScope::Instance, material->GetUniformValues());
		}
	}

	void VulkanRenderContext::SetInstanceUniforms(uint64 instanceID, std::span<const ShaderUniformValue> uniforms)
	{
		CocoAssert(_globalState.has_value(), "No shader was bound")

		uint64 dataID = Math::CombineHashes(_globalState->BoundShader->GetID(), instanceID);
		_instanceState.emplace(dataID);

		SetUniforms(dataID, 0, UniformScope::Instance, uniforms);
	}

	void VulkanRenderContext::SetDrawUniforms(std::span<const ShaderUniformValue> uniforms)
	{
		_drawUniforms = std::vector<ShaderUniformValue>(uniforms.begin(), uniforms.end());
	}

	void VulkanRenderContext::Begin(const CompiledRenderPipeline& pipeline, RenderView& renderView, VulkanRenderFrame& renderFrame)
	{
		_renderCompletedFence->Reset();

		// Get the images from the render targets
		std::span<const RenderTarget> rts = renderView.GetRenderTargets();
		std::vector<const VulkanImage*> vulkanImages(rts.size());
		std::vector<uint32> resolveImageIndices;
		std::vector<VkClearValue> clearValues(rts.size());

		for (uint32 i = 0; i < rts.size(); i++)
		{
			// Add attachment images
			CocoAssert(rts[i].MainImage.IsValid(), FormatString("RenderTarget {}'s image was invalid", i))
			vulkanImages.at(i) = static_cast<const VulkanImage*>(rts[i].MainImage.Get());

			// Add the clear value for the attachment
			const RenderPassAttachment& attachment = pipeline.Attachments.at(i);
			clearValues.at(i) = ToVkClearValue(rts[i].ClearValue, attachment.PixelFormat);

			// Setup the resolve image if one exists
			if (rts[i].ResolveImage.IsValid())
			{
				resolveImageIndices.push_back(i);
				vulkanImages.push_back(static_cast<const VulkanImage*>(rts[i].ResolveImage.Get()));
				clearValues.push_back(clearValues.at(i));
			}
		}

		const RectInt& viewportRect = renderView.GetViewportRect();
		SizeInt viewportSize = viewportRect.GetSize();
		VulkanGraphicsDeviceCache& deviceCache = _device.GetCache();
		VulkanRenderPass& renderPass = deviceCache.GetOrCreateRenderPass(pipeline, renderView.GetSampleCount(), resolveImageIndices);
		VulkanFramebuffer& framebuffer = deviceCache.GetOrCreateFramebuffer(renderPass, vulkanImages);
		_renderOperation.emplace(pipeline, renderView, *_commandBuffer, renderPass, renderFrame);
		
		_commandBuffer->Begin(true, false);
		
		AddPreRenderPassAttachmentTransitions();

		// Start the first render pass
		VkRenderPassBeginInfo beginInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		beginInfo.renderPass = renderPass.GetRenderPass();
		beginInfo.framebuffer = framebuffer.GetFramebuffer();

		beginInfo.renderArea.offset.x = static_cast<uint32_t>(viewportRect.GetLeft());
		beginInfo.renderArea.offset.y = static_cast<uint32_t>(viewportRect.GetBottom());
		beginInfo.renderArea.extent.width = static_cast<uint32_t>(viewportSize.Width);
		beginInfo.renderArea.extent.height = static_cast<uint32_t>(viewportSize.Height);

		beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		beginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(_commandBuffer->GetCmdBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Since viewport and scissors are dynamic, we must set them at least once
		SetViewportRect(viewportRect);
		SetScissorRect(viewportRect);

		// TODO: dynamic line width?
		vkCmdSetLineWidth(_commandBuffer->GetCmdBuffer(), 1.0f);
	}

	void VulkanRenderContext::BeginNextPass()
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		_globalState.reset();
		_instanceState.reset();
		_drawUniforms.clear();

		vkCmdNextSubpass(_renderOperation->CommandBuffer.GetCmdBuffer(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderContext::End(std::span<std::pair<Ref<VulkanGraphicsSemaphore>, VkPipelineStageFlags>> waitSemaphores, std::span<Ref<VulkanGraphicsSemaphore>> signalSemaphores)
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		vkCmdEndRenderPass(_renderOperation->CommandBuffer.GetCmdBuffer());

		AddPostRenderPassAttachmentTransitions();

		_commandBuffer->EndAndSubmit(waitSemaphores, signalSemaphores, _renderCompletedFence);
		_renderOperation.reset();
	}

	void VulkanRenderContext::Reset()
	{
		WaitForRenderToComplete();

		_globalState.reset();
		_instanceState.reset();
		_drawUniforms.clear();

		_renderOperation.reset();

		_commandBuffer->Reset();

		_renderStats = RenderContextStats();

		// Free descriptor sets and unused pools
		for (auto& poolKvp : _descriptorPools)
		{
			VulkanDescriptorSetPool& pool = poolKvp.second;
			pool.FreeSets();
			pool.PurgeUnusedPools(_stalePoolThreshold);
		}

		// Remove pool sets if they're unused
		std::erase_if(_descriptorPools,
			[](const auto& kvp)
			{
				return kvp.second.GetPoolCount() == 0;
			});
	}

	void VulkanRenderContext::CreateCommandBuffer()
	{
		VulkanQueue* graphicsQueue = _device.GetQueue(VulkanQueueType::Graphics);
		CocoAssert(graphicsQueue, "Device does not support graphics operations")
	
		_commandBuffer = CreateUniqueRef<VulkanCommandBuffer>(graphicsQueue->GetCommandPool()->Allocate(true));
	}

	void VulkanRenderContext::FreeCommandBuffer()
	{
		if (!_commandBuffer)
			return;
	
		VulkanQueue* graphicsQueue = _device.GetQueue(VulkanQueueType::Graphics);
		graphicsQueue->GetCommandPool()->Free(*_commandBuffer);
		_commandBuffer.reset();
	}

	void VulkanRenderContext::AddPreRenderPassAttachmentTransitions()
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		//VkMemoryBarrier memoryBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
		//memoryBarrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
		//	VK_ACCESS_INDEX_READ_BIT |
		//	VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
		//	VK_ACCESS_UNIFORM_READ_BIT |
		//	VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_SHADER_READ_BIT |
		//	VK_ACCESS_SHADER_WRITE_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_TRANSFER_READ_BIT |
		//	VK_ACCESS_TRANSFER_WRITE_BIT |
		//	VK_ACCESS_HOST_READ_BIT |
		//	VK_ACCESS_HOST_WRITE_BIT;
		//
		//memoryBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
		//	VK_ACCESS_INDEX_READ_BIT |
		//	VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
		//	VK_ACCESS_UNIFORM_READ_BIT |
		//	VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_SHADER_READ_BIT |
		//	VK_ACCESS_SHADER_WRITE_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_TRANSFER_READ_BIT |
		//	VK_ACCESS_TRANSFER_WRITE_BIT |
		//	VK_ACCESS_HOST_READ_BIT |
		//	VK_ACCESS_HOST_WRITE_BIT;
		//
		//vkCmdPipelineBarrier(_renderOperation->CommandBuffer.GetCmdBuffer(),
		//	VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		//	0,
		//	1, &memoryBarrier,
		//	0, nullptr,
		//	0, nullptr);
	
		const auto& attachmentFormats = _renderOperation->Pipeline.Attachments;
		std::span<const RenderTarget> rts = _renderOperation->View.GetRenderTargets();

		VulkanQueue* queue = _device.GetQueue(VulkanQueueType::Graphics);
	
		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const RenderPassAttachment& attachment = attachmentFormats.at(i);
	
			const RenderTarget& rt = rts[i];
			VkImageLayout layout = ToVkImageLayout(attachment.PixelFormat);
	
			VulkanImage* image = static_cast<VulkanImage*>(rt.MainImage.Get());
			image->TransitionLayout(_renderOperation->CommandBuffer, *queue, *queue, layout);
	
			if (rt.ResolveImage.IsValid())
			{
				image = static_cast<VulkanImage*>(rt.ResolveImage.Get());
				image->TransitionLayout(_renderOperation->CommandBuffer, *queue, *queue, layout);
			}
		}
	}

	void VulkanRenderContext::AddPostRenderPassAttachmentTransitions()
	{
		CocoAssert(_renderOperation.has_value(), "RenderContext was not rendering")

		const auto& attachmentFormats = _renderOperation->Pipeline.Attachments;
		std::span<const RenderTarget> rts = _renderOperation->View.GetRenderTargets();

		VulkanQueue* queue = _device.GetQueue(VulkanQueueType::Graphics);

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const RenderPassAttachment& attachment = attachmentFormats.at(i);
			VkImageLayout layout = ToVkImageLayout(attachment.PixelFormat);
			const RenderTarget& rt = rts[i];

			VulkanImage* image = static_cast<VulkanImage*>(rt.MainImage.Get());

			// Set the new layout for the image
			image->_imageInfo.CurrentLayout = layout;

			// Do the same for the resolve image, if one exists
			if (rt.ResolveImage.IsValid())
			{
				image = static_cast<VulkanImage*>(rt.ResolveImage.Get());
				image->_imageInfo.CurrentLayout = layout;
			}

			const ImageDescription& imageDesc = image->GetDescription();

			// Transition any attachments that can be sampled from in shaders
			if ((imageDesc.UsageFlags & ImageUsageFlags::Sampled) == ImageUsageFlags::Sampled)
			{
				image->TransitionLayout(_renderOperation->CommandBuffer, *queue, *queue, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
		}

		//VkMemoryBarrier memoryBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
		//memoryBarrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
		//	VK_ACCESS_INDEX_READ_BIT |
		//	VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
		//	VK_ACCESS_UNIFORM_READ_BIT |
		//	VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_SHADER_READ_BIT |
		//	VK_ACCESS_SHADER_WRITE_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_TRANSFER_READ_BIT |
		//	VK_ACCESS_TRANSFER_WRITE_BIT |
		//	VK_ACCESS_HOST_READ_BIT |
		//	VK_ACCESS_HOST_WRITE_BIT;
		//
		//memoryBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
		//	VK_ACCESS_INDEX_READ_BIT |
		//	VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
		//	VK_ACCESS_UNIFORM_READ_BIT |
		//	VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_SHADER_READ_BIT |
		//	VK_ACCESS_SHADER_WRITE_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
		//	VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
		//	VK_ACCESS_TRANSFER_READ_BIT |
		//	VK_ACCESS_TRANSFER_WRITE_BIT |
		//	VK_ACCESS_HOST_READ_BIT |
		//	VK_ACCESS_HOST_WRITE_BIT;
		//
		//vkCmdPipelineBarrier(_renderOperation->CommandBuffer.GetCmdBuffer(),
		//	VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		//	0,
		//	1, &memoryBarrier,
		//	0, nullptr,
		//	0, nullptr);
	}

	VkDescriptorSet VulkanRenderContext::GetOrCreateDescriptorSet(uint64 dataID, UniformScope scope, VulkanDescriptorSetLayout& descriptorSetLayout)
	{
		// Early out if no descriptors
		if (descriptorSetLayout.IsEmpty())
			return nullptr;

		uint64 setKey = Math::CombineHashes(static_cast<int>(scope), dataID);

		// If the set is already allocated, just use that
		VulkanDescriptorSetPool& pool = _descriptorPools.try_emplace(descriptorSetLayout.ID, _device, descriptorSetLayout).first->second;
		VkDescriptorSet set = nullptr;
		if (pool.TryGetAllocatedSet(setKey, set))
			return set;

		const ShaderUniformLayout& layout = descriptorSetLayout.GetUniformLayout();

		set = pool.AllocateDescriptorSet(setKey);

		if (scope != UniformScope::Draw)
		{
			// Write descriptors using the cached data
			const CachedVulkanUniformData& cachedData = _renderOperation->RenderFrame.GetUniformCache().CreateOrGetData(dataID, scope, layout);
			WriteDescriptorSetData(set, descriptorSetLayout, cachedData);
			WriteDescriptorSetTextures(set, descriptorSetLayout, cachedData.Textures);
		}
		else
		{
			auto textures = layout.GetTextures(_drawUniforms);
			std::unordered_map<string, WeakSharedRef<Texture>> textureRefs;
			for (const auto& kvp : textures)
			{
				textureRefs.try_emplace(kvp.first, kvp.second);
			}

			WriteDescriptorSetTextures(set, descriptorSetLayout, textureRefs);
		}

		return set;
	}

	void VulkanRenderContext::BindDescriptorSet(VkDescriptorSet set, uint32 offset)
	{
		if (!set)
			return;

		vkCmdBindDescriptorSets(
			_renderOperation->CommandBuffer.GetCmdBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			_globalState->BoundPipeline.GetPipelineLayout(),
			offset,
			1, &set,
			0, 0);
	}

	bool VulkanRenderContext::FlushStateChanges()
	{
		CocoAssert(_globalState.has_value(), "No shader was bound")

		VulkanShader& shader = _globalState->BoundVulkanShader;

		const ShaderUniformLayout& instanceLayout = shader.GetUniformLayout(UniformScope::Instance);

		if (instanceLayout != ShaderUniformLayout::Empty)
		{
			if (!_instanceState.has_value())
			{
				_instanceState.emplace(shader.ID);
			}

			VulkanDescriptorSetLayout& instanceSetLayout = _device.GetCache().GetOrCreateDescriptorSetLayout(instanceLayout, true);
			_instanceState->InstanceSet = GetOrCreateDescriptorSet(_instanceState->InstanceID, UniformScope::Instance, instanceSetLayout);
			BindDescriptorSet(_instanceState->InstanceSet, 2);
		}

		const ShaderUniformLayout& drawLayout = shader.GetUniformLayout(UniformScope::Draw);

		if (drawLayout != ShaderUniformLayout::Empty)
		{
			VulkanDescriptorSetLayout& drawSetLayout = _device.GetCache().GetOrCreateDescriptorSetLayout(drawLayout, false);

			// TODO: ensure the risk of collisions between draw data IDs is small
			uint64 dataID = static_cast<uint64>(Random::Global.RandomValue() * Math::MaxValue<uint64>());
			VkDescriptorSet drawSet = GetOrCreateDescriptorSet(dataID, UniformScope::Draw, drawSetLayout);
			BindDescriptorSet(drawSet, 3);
			FlushPushConstants(drawLayout);
		}

		_drawUniforms.clear();

		return true;
	}

	void VulkanRenderContext::FlushPushConstants(const ShaderUniformLayout& drawLayout)
	{
		CocoAssert(_globalState.has_value(), "No shader was bound")

		if (!drawLayout.HasUniformsOfType(true, false))
			return;

		VkShaderStageFlags stageFlags = 0;

		for (const ShaderUniform* u : drawLayout.GetUniforms(true, false))
		{
			VkShaderStageFlags uniformStage = ToVkShaderStageFlags(u->BindingPoints);

			stageFlags |= uniformStage;
		}

		std::vector<uint8> data = drawLayout.GetBufferData(_drawUniforms);

		vkCmdPushConstants(
			_renderOperation->CommandBuffer.GetCmdBuffer(),
			_globalState->BoundPipeline.GetPipelineLayout(),
			stageFlags,
			0,
			static_cast<uint32_t>(data.size()),
			data.data());
	}

	void VulkanRenderContext::SetUniforms(uint64 dataID, uint64 dataVersion, UniformScope scope, std::span<const ShaderUniformValue> uniforms)
	{
		CocoAssert(_renderOperation.has_value(), "The render operation was not set. Was Begin() called for this VulkanRenderContext?")

		const ShaderUniformLayout* layout = nullptr;

		switch (scope)
		{
		case UniformScope::Global:
		{
			dataID = _globalDataID;
			layout = &_renderOperation->View.GetGlobalUniformLayout();
			break;
		case UniformScope::ShaderGlobal:
		case UniformScope::Instance:
		{
			CocoAssert(_globalState.has_value(), "No shader was bound")
			VulkanShader& shader = _globalState->BoundVulkanShader;

			if (scope == UniformScope::ShaderGlobal)
				dataID = shader.ID;

			layout = &shader.GetUniformLayout(scope);
			break;
		}
		}
		default:
			return;
		}

		CocoAssert(layout, "Shader uniform layout was null")

		_renderOperation->RenderFrame.GetUniformCache().CreateOrUpdateData(dataID, dataVersion, scope, *layout, uniforms);
	}

	void VulkanRenderContext::WriteDescriptorSetData(VkDescriptorSet set, const VulkanDescriptorSetLayout& descriptorSetLayout, const CachedVulkanUniformData& cachedData)
	{
		const ShaderUniformLayout& uniformLayout = descriptorSetLayout.GetUniformLayout();
		if (!uniformLayout.HasUniformsOfType(true, false))
			return;

		std::vector<VkWriteDescriptorSet> descriptorWrites = descriptorSetLayout.GetDescriptorWrites();
		// TODO: support arbitrary data writes
		descriptorWrites.resize(1);

		std::vector<VkDescriptorBufferInfo> bufferInfos;

		// Write uniform buffer binding
		VkDescriptorBufferInfo& bufferInfo = bufferInfos.emplace_back();
		bufferInfo.buffer = cachedData.Buffer->GetBuffer();
		bufferInfo.offset = cachedData.AllocBlock->Offset;

		bufferInfo.range = cachedData.AllocBlock->Size;

		VkWriteDescriptorSet& write = descriptorWrites.front();
		write.dstSet = set;
		write.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(_device.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void VulkanRenderContext::WriteDescriptorSetTextures(
		VkDescriptorSet set, 
		const VulkanDescriptorSetLayout& descriptorSetLayout, 
		const std::unordered_map<string, WeakSharedRef<Texture>>& textures)
	{
		const ShaderUniformLayout& uniformLayout = descriptorSetLayout.GetUniformLayout();
		if (!uniformLayout.HasUniformsOfType(false, true))
			return;

		std::vector<VkWriteDescriptorSet> descriptorWrites = descriptorSetLayout.GetDescriptorWrites();
		std::vector<VkDescriptorImageInfo> imageInfos;
		std::vector<const ShaderUniform*> layoutUniforms = uniformLayout.GetUniforms(false, true);
		auto layoutUniformIt = layoutUniforms.begin();

		auto it = descriptorWrites.begin();
		while(it != descriptorWrites.end())
		{
			VkWriteDescriptorSet& write = *it;

			if (write.descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
			{
				it = descriptorWrites.erase(it);
				continue;
			}

			const ShaderUniform* layoutUniform = *layoutUniformIt;

			Ref<Image> image;
			Ref<ImageSampler> sampler;

			auto textureMapIt = textures.find(layoutUniform->Name);
			if (textureMapIt != textures.end() && !textureMapIt->second.expired())
			{
				SharedRef<Texture> tex = textureMapIt->second.lock();

				image = tex->GetImage();
				sampler = tex->GetImageSampler();
			}

			if (!image.IsValid() || !sampler.IsValid())
			{
				const RenderService* rendering = RenderService::Get();
				CocoAssert(rendering, "RenderService singleton was null")

				SharedRef<Texture> defaultTexture;
				DefaultTextureType textureType = DefaultTextureType::Checkered;

				if (const DefaultTextureType* t = std::any_cast<DefaultTextureType>(&layoutUniform->DefaultValue))
					textureType = *t;

				switch (textureType)
				{
				case DefaultTextureType::White:
					defaultTexture = rendering->GetDefaultDiffuseTexture();
					break;
				case DefaultTextureType::FlatNormal:
					defaultTexture = rendering->GetDefaultNormalTexture();
					break;
				case DefaultTextureType::Checkered:
				default:
					defaultTexture = rendering->GetDefaultCheckerTexture();
					break;
				}

				image = defaultTexture->GetImage();
				sampler = defaultTexture->GetImageSampler();
			}

			CocoAssert(image.IsValid(), "The image reference is empty")
			CocoAssert(sampler.IsValid(), "The image sampler reference is empty")

			VulkanImage* vulkanImage = static_cast<VulkanImage*>(image.Get());
			VulkanImageSampler* vulkanSampler = static_cast<VulkanImageSampler*>(sampler.Get());

			// Write texture data
			VkDescriptorImageInfo& imageInfo = imageInfos.emplace_back();
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanImage->GetNativeView();
			imageInfo.sampler = vulkanSampler->GetSampler();

			write.dstSet = set;
			write.pImageInfo = &imageInfo;

			++layoutUniformIt;
			++it;
		}

		vkUpdateDescriptorSets(_device.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}