#include "Renderpch.h"
#include "VulkanRenderContext.h"
#include "VulkanGraphicsDevice.h"
#include "VulkanImage.h"
#include "VulkanUtils.h"
#include "VulkanCommandBuffer.h"
#include "CachedResources/VulkanFramebuffer.h"
#include "CachedResources/VulkanRenderPass.h"
#include "CachedResources/VulkanPipeline.h"
#include "CachedResources/VulkanRenderContextCache.h"
#include "CachedResources/VulkanShader.h"
#include "VulkanBuffer.h"

#include <Coco/Core/Math/Random.h>
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	BoundGlobalState::BoundGlobalState(const string& shaderName) :
		ShaderName(shaderName),
		Pipeline(nullptr),
		DescriptorSet(nullptr)
	{}

	BoundInstanceState::BoundInstanceState(uint64 instanceID) :
		InstanceID(instanceID),
		DescriptorSet(nullptr)
	{}

	VulkanContextRenderOperation::VulkanContextRenderOperation(
		VulkanFramebuffer& framebuffer, 
		VulkanRenderPass& renderPass) :
		Framebuffer(framebuffer),
		RenderPass(renderPass),
		ViewportRect{},
		ScissorRect{},
		StateChanges{},
		GlobalState{},
		InstanceState{}
	{}

	VulkanRenderContext::VulkanRenderContext(const GraphicsDeviceResourceID& id) :
		GraphicsDeviceResource<VulkanGraphicsDevice>(id),
		_vulkanRenderOperation{},
		_renderStartSemaphore(CreateManagedRef<VulkanGraphicsSemaphore>(id)),
		_renderCompletedSemaphore(CreateManagedRef<VulkanGraphicsSemaphore>(id)),
		_renderCompletedFence(CreateManagedRef<VulkanGraphicsFence>(id, false)),
		_waitOnSemaphores{},
		_renderCompletedSignalSemaphores{},
		_commandBuffer{},
		_deviceCache(_device.GetCache()),
		_globalDescriptorSet(std::make_pair(nullptr, nullptr))
	{
		DeviceQueue* graphicsQueue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (!graphicsQueue)
		{
			throw std::exception("A graphics queue is required for rendering operations");
		}

		_commandBuffer = graphicsQueue->Pool.Allocate(true);
	}

	VulkanRenderContext::~VulkanRenderContext()
	{
		_renderStartSemaphore.Invalidate();
		_renderCompletedSemaphore.Invalidate();
		_renderCompletedFence.Invalidate();

		DeviceQueue* graphicsQueue = _device.GetQueue(DeviceQueue::Type::Graphics);
		if (graphicsQueue)
		{
			graphicsQueue->Pool.Free(*_commandBuffer);
		}

		_commandBuffer.reset();
	}

	void VulkanRenderContext::WaitForRenderingToComplete()
	{
		if (_currentState == RenderContextState::RenderCompleted)
			return;

		_renderCompletedFence->Wait(Math::MaxValue<uint64>());
		_currentState = RenderContextState::RenderCompleted;
	}

	bool VulkanRenderContext::CheckForRenderingComplete()
	{
		if (_currentState == RenderContextState::EndedRender)
		{
			if (_renderCompletedFence->IsSignaled())
				_currentState = RenderContextState::RenderCompleted;
		}

		return _currentState == RenderContextState::RenderCompleted;
	}

	void VulkanRenderContext::AddWaitOnSemaphore(Ref<GraphicsSemaphore> semaphore)
	{
		if (!semaphore.IsValid())
		{
			CocoError("Wait on semaphore was invalid")
				return;
		}

		Ref<VulkanGraphicsSemaphore> vulkanSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(semaphore);
		_waitOnSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::AddRenderCompletedSignalSemaphore(Ref<GraphicsSemaphore> semaphore)
	{
		if (!semaphore.IsValid())
		{
			CocoError("Signal semaphore was invalid")
				return;
		}

		Ref<VulkanGraphicsSemaphore> vulkanSemaphore = static_cast<Ref<VulkanGraphicsSemaphore>>(semaphore);
		_renderCompletedSignalSemaphores.push_back(vulkanSemaphore);
	}

	void VulkanRenderContext::SetViewportRect(const RectInt& viewportRect)
	{
		Assert(_vulkanRenderOperation.has_value())
		_vulkanRenderOperation->ViewportRect = viewportRect;

		SizeInt s = viewportRect.GetSize();

		VkViewport viewport{};
		viewport.x = static_cast<float>(viewportRect.GetLeft());
		viewport.y = static_cast<float>(viewportRect.GetBottom());
		viewport.width = static_cast<float>(s.Width);
		viewport.height = static_cast<float>(s.Height);
		viewport.minDepth = 0.0f; // TODO: configurable min/max depth
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(_commandBuffer->GetCmdBuffer(), 0, 1, &viewport);
	}

	void VulkanRenderContext::SetScissorRect(const RectInt& scissorRect)
	{
		Assert(_vulkanRenderOperation.has_value())
		_vulkanRenderOperation->ScissorRect = scissorRect;

		SizeInt s = scissorRect.GetSize();

		VkRect2D scissor{};
		scissor.offset.x = static_cast<uint32_t>(scissorRect.GetLeft());
		scissor.offset.y = static_cast<uint32_t>(scissorRect.GetBottom());
		scissor.extent.width = static_cast<uint32_t>(s.Width);
		scissor.extent.height = static_cast<uint32_t>(s.Height);

		vkCmdSetScissor(_commandBuffer->GetCmdBuffer(), 0, 1, &scissor);
	}

	void VulkanRenderContext::SetShader(const string& shaderName)
	{
		Assert(_vulkanRenderOperation.has_value())
		if (_vulkanRenderOperation->GlobalState.has_value() && 
			_vulkanRenderOperation->GlobalState->ShaderName == shaderName)
			return;

		_vulkanRenderOperation->GlobalState.emplace(shaderName);

		// Clear shader-specific uniforms
		_instanceUniforms.Clear();
		_drawUniforms.Clear();

		// Reset shader-specific bindings
		_vulkanRenderOperation->InstanceState.reset();

		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Shader);
	}

	void VulkanRenderContext::SetMaterial(const MaterialData& material)
	{
		Assert(_vulkanRenderOperation.has_value())

		if (_vulkanRenderOperation->InstanceState.has_value() && _vulkanRenderOperation->InstanceState->InstanceID == material.ID)
			return;

		_instanceUniforms = material.UniformData;

		_vulkanRenderOperation->InstanceState.emplace(material.ID);
		_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::Instance);
	}

	void VulkanRenderContext::DrawIndexed(const MeshData& mesh, uint64 firstIndexOffset, uint64 indexCount)
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(mesh.IndexBuffer.IsValid())
		Assert(mesh.VertexBuffer.IsValid())

		if (!FlushStateChanges())
		{
			CocoError("Failed to setup state for mesh. Skipping...")
				return;
		}

		VkCommandBuffer cmdBuffer = _commandBuffer->GetCmdBuffer();

 		uint64 vertexPositionSize = mesh.VertexCount * GetBufferDataTypeSize(BufferDataType::Float3);
		bool hasOptionalAttributes = mesh.Format.AdditionalAttributes != VertexAttrFlags::None;

		// Bind the vertex buffer
		std::array<VkDeviceSize, 2> offsets = { 0, vertexPositionSize };
		const VulkanBuffer& vertexBuffer = static_cast<const VulkanBuffer&>(*mesh.VertexBuffer);
		std::array<VkBuffer, 2> vertexBuffers = { vertexBuffer.GetBuffer(), vertexBuffer.GetBuffer() };
		vkCmdBindVertexBuffers(cmdBuffer, 0, hasOptionalAttributes ? 2 : 1, vertexBuffers.data(), offsets.data());

		// Bind the index buffer
		const VulkanBuffer& indexBuffer = static_cast<const VulkanBuffer&>(*mesh.IndexBuffer);
		vkCmdBindIndexBuffer(cmdBuffer, indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Draw the mesh
		vkCmdDrawIndexed(cmdBuffer,
			static_cast<uint32>(indexCount),
			1,
			static_cast<uint32>(firstIndexOffset),
			0, 0);

		_stats.VertexCount += mesh.VertexCount;
		_stats.TrianglesDrawn += indexCount / 3;
		_stats.DrawCalls++;

		_drawUniforms.Clear();
	}

	void VulkanRenderContext::SetGlobalBufferData(ShaderUniformData::UniformKey key, uint64 offset, const void* data, uint64 dataSize)
	{
		Assert(_renderView != nullptr)

		VulkanRenderContextCache& cache = _deviceCache.GetOrCreateContextCache(ID);
		VulkanGlobalUniformData& globalData = cache.GetOrCreateGlobalUniformData(_renderView->GetGlobalUniformLayout());
		globalData.SetBufferUniformData(key, offset, data, dataSize);
	}

	void VulkanRenderContext::SetShaderGlobalBufferData(ShaderUniformData::UniformKey key, uint64 offset, const void* data, uint64 dataSize)
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(_vulkanRenderOperation->GlobalState.has_value())
		Assert(!_vulkanRenderOperation->GlobalState->ShaderName.empty())

		VulkanShader& shader = GetBoundShader();
		VulkanShaderUniformData& uniformData = GetUniformDataForShader(shader);

		uniformData.SetBufferUniformData(key, offset, data, dataSize);

		UniformChanged(UniformScope::ShaderGlobal, key);
	}

	bool VulkanRenderContext::BeginImpl()
	{
		try
		{
			_renderCompletedSignalSemaphores.push_back(_renderCompletedSemaphore);

			// Get the images from the render targets
			std::span<const RenderTarget> rts = _renderView->GetRenderTargets();
			std::vector<const VulkanImage*> vulkanImages(rts.size());
			std::vector<uint8> resolveImageIndices;

			// Add primary attachments
			for (uint8 i = 0; i < rts.size(); i++)
			{
				Assert(rts[i].MainImage.IsValid())
				vulkanImages.at(i) = static_cast<const VulkanImage*>(rts[i].MainImage.Get());

				if (rts[i].ResolveImage.IsValid())
				{
					resolveImageIndices.push_back(i);
				}
			}
			
			std::vector<VkClearValue> clearValues(rts.size());

			// Set clear clear color for each render target
			for (size_t i = 0; i < clearValues.size(); i++)
			{
				const CompiledPipelineAttachment& attachment = _renderOperation->Pipeline.Attachments.at(i);
				SetClearValue(rts[i].ClearValue, attachment.PixelFormat, clearValues.at(i));
			}

			// Add any resolve attachments
			for (const uint8& i : resolveImageIndices)
			{
				vulkanImages.push_back(static_cast<const VulkanImage*>(rts[i].ResolveImage.Get()));
				clearValues.push_back(clearValues.at(i));
			}

 			const RectInt& viewportRect = _renderView->GetViewportRect();
			SizeInt viewportSize = viewportRect.GetSize();

			VulkanRenderPass& renderPass = _deviceCache.GetOrCreateRenderPass(_renderOperation->Pipeline, _renderView->GetMSAASamples(), resolveImageIndices);
			VulkanRenderContextCache& cache = _deviceCache.GetOrCreateContextCache(ID);
			VulkanFramebuffer& framebuffer = cache.GetOrCreateFramebuffer(viewportSize, renderPass, vulkanImages);

			// Setup the Vulkan-specific render operation
			_vulkanRenderOperation.emplace(VulkanContextRenderOperation(framebuffer, renderPass));

			_vulkanRenderOperation->ViewportRect = viewportRect;
			_vulkanRenderOperation->ScissorRect = _renderView->GetScissorRect();

			_commandBuffer->Begin(true, false);

			AddPreRenderPassImageTransitions();

			// Start the first render pass
			VkRenderPassBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
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
			SetViewportRect(_vulkanRenderOperation->ViewportRect);
			SetScissorRect(_vulkanRenderOperation->ScissorRect);

			// TODO: dynamic line width?
			vkCmdSetLineWidth(_commandBuffer->GetCmdBuffer(), 1.0f);

			const GlobalShaderUniformLayout& globalLayout = _renderView->GetGlobalUniformLayout();

			// Upload global state data
			if (globalLayout.Hash != ShaderUniformLayout::EmptyHash)
			{
				VulkanGlobalUniformData& uniformData = cache.GetOrCreateGlobalUniformData(globalLayout);
				_globalDescriptorSet = std::make_pair(&uniformData.GetDescriptorSetLayout(), uniformData.PrepareData(_globalUniforms));
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Could not begin VulkanRenderContext: {}", ex.what())
			return false;
		}

		return true;
	}

	bool VulkanRenderContext::BeginNextPassImpl()
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(_currentState == RenderContextState::InRender)

		_vulkanRenderOperation->GlobalState.reset();
		_vulkanRenderOperation->InstanceState.reset();

		vkCmdNextSubpass(_commandBuffer->GetCmdBuffer(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		return true;
	}

	void VulkanRenderContext::EndImpl()
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(_currentState == RenderContextState::InRender)

		vkCmdEndRenderPass(_commandBuffer->GetCmdBuffer());

		AddPostRenderPassImageTransitions();

		_commandBuffer->EndAndSubmit(
			&_waitOnSemaphores,
			&_renderCompletedSignalSemaphores,
			_renderCompletedFence);
	}

	void VulkanRenderContext::ResetImpl()
	{
		WaitForRenderingToComplete();

		_vulkanRenderOperation.reset();

		_waitOnSemaphores.clear();
		_renderCompletedSignalSemaphores.clear();
		_renderCompletedFence->Reset();

		_globalDescriptorSet = std::make_pair(nullptr, nullptr);
	}

	void VulkanRenderContext::UniformChanged(UniformScope scope, ShaderUniformData::UniformKey key)
	{
		if (scope == UniformScope::Draw)
			_vulkanRenderOperation->StateChanges.emplace(VulkanContextRenderOperation::StateChangeType::DrawUniform);
	}

	void VulkanRenderContext::AddPreRenderPassImageTransitions()
	{
		Assert(_vulkanRenderOperation.has_value())

		const auto& attachmentFormats = _renderOperation->Pipeline.Attachments;
		std::span<RenderTarget> rts = _renderView->GetRenderTargets();

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const CompiledPipelineAttachment& attachment = attachmentFormats.at(i);

			// Don't bother transitioning layouts for attachments that are cleared as Vulkan will handle that for us
			if ((attachment.Options & AttachmentOptionFlags::Clear) == AttachmentOptionFlags::Clear)
				continue;

			RenderTarget& rt = rts[i];
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);

			VulkanImage* image = static_cast<VulkanImage*>(rt.MainImage.Get());
			image->TransitionLayout(*_commandBuffer, layout);

			if (rt.ResolveImage.IsValid())
			{
				image = static_cast<VulkanImage*>(rt.ResolveImage.Get());
				image->TransitionLayout(*_commandBuffer, layout);
			}
		}
	}

	void VulkanRenderContext::AddPostRenderPassImageTransitions()
	{
		Assert(_vulkanRenderOperation.has_value())

		const auto& attachmentFormats = _renderOperation->Pipeline.Attachments;
		std::span<RenderTarget> rts = _renderView->GetRenderTargets();

		for (size_t i = 0; i < attachmentFormats.size(); i++)
		{
			const CompiledPipelineAttachment& attachment = attachmentFormats.at(i);
			VkImageLayout layout = ToAttachmentLayout(attachment.PixelFormat);
			RenderTarget& rt = rts[i];

			VulkanImage* image = static_cast<VulkanImage*>(rt.MainImage.Get());

			AddPostRenderPassImageTransitions(layout, *image);

			// Do the same for the resolve image, if one exists
			if (rt.ResolveImage.IsValid())
			{
				image = static_cast<VulkanImage*>(rt.ResolveImage.Get());
				
				AddPostRenderPassImageTransitions(layout, *image);
			}
		}
	}

	void VulkanRenderContext::AddPostRenderPassImageTransitions(VkImageLayout currentLayout, VulkanImage& image)
	{
		// Since Vulkan automatically transitions layouts between passes, update the image's layout to match the layouts of the attachments
		image._imageData.CurrentLayout = currentLayout;

		const ImageDescription imageDesc = image.GetDescription();

		// Transition any attachments that will be presented
		if ((imageDesc.UsageFlags & ImageUsageFlags::Presented) == ImageUsageFlags::Presented)
		{
			image.TransitionLayout(*_commandBuffer, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		}
		// Transition any attachments that can be sampled from in shaders
		else if ((imageDesc.UsageFlags & ImageUsageFlags::Sampled) == ImageUsageFlags::Sampled)
		{
			image.TransitionLayout(*_commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	bool VulkanRenderContext::FlushStateChanges()
	{
		Assert(_vulkanRenderOperation.has_value())

		// Early out if no state changes since last draw
		if (_vulkanRenderOperation->StateChanges.size() == 0)
			return true;

		bool stateBound = true;

		try
		{
			if (!_vulkanRenderOperation->GlobalState.has_value())
			{
				CocoError("No global state was bound")
				_vulkanRenderOperation->GlobalState.emplace(RenderService::sErrorShaderName);
			}

			BoundGlobalState& globalState = _vulkanRenderOperation->GlobalState.value();

			VulkanShader& shader = GetBoundShader();
			VulkanShaderUniformData& uniformData = GetUniformDataForShader(shader);

			VkCommandBuffer cmdBuffer = _commandBuffer->GetCmdBuffer();

			// Bind the new shader if it changed
			if (_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Shader))
			{
				globalState.Pipeline = &_deviceCache.GetOrCreatePipeline(
					_vulkanRenderOperation->RenderPass,
					_renderOperation->CurrentPassIndex,
					shader,
					_globalDescriptorSet.first
				);

				vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, globalState.Pipeline->GetPipeline());

				if (_globalDescriptorSet.second)
				{
					// Bind the global descriptor set
					vkCmdBindDescriptorSets(
						cmdBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						globalState.Pipeline->GetPipelineLayout(),
						0,
						1, &_globalDescriptorSet.second,
						0, 0);
				}

				if (shader.HasScope(UniformScope::ShaderGlobal))
				{
					globalState.DescriptorSet = uniformData.PrepareGlobalData(_globalShaderUniforms);

					Assert(globalState.DescriptorSet != nullptr)

					// Bind the shader global descriptor set
					vkCmdBindDescriptorSets(
						cmdBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						globalState.Pipeline->GetPipelineLayout(),
						1,
						1, &globalState.DescriptorSet,
						0, 0);
				}
			}

			Assert(globalState.Pipeline != nullptr)

			// Bind the instance descriptors if the instance changed or the shader needs instance data
			if (shader.HasScope(UniformScope::Instance) &&
				(_vulkanRenderOperation->StateChanges.contains(VulkanContextRenderOperation::StateChangeType::Instance) || !_vulkanRenderOperation->InstanceState.has_value()))
			{
				if (!_vulkanRenderOperation->InstanceState.has_value())
				{
					_vulkanRenderOperation->InstanceState.emplace(0);
				}

				BoundInstanceState& instanceState = _vulkanRenderOperation->InstanceState.value();

				instanceState.DescriptorSet = uniformData.PrepareInstanceData(shader, instanceState.InstanceID, _instanceUniforms, instanceState.InstanceID != 0);

				Assert(instanceState.DescriptorSet != nullptr)

				//// Bind the instance descriptor set
				vkCmdBindDescriptorSets(
					cmdBuffer,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					globalState.Pipeline->GetPipelineLayout(),
					2, 
					1, &instanceState.DescriptorSet,
					0, 0);
			}

			// Bind the draw descriptors if a draw uniform changed or the shader needs draw data
			if (shader.HasScope(UniformScope::Draw))
			{
				VkDescriptorSet drawSet = uniformData.PrepareDrawData(shader, *_commandBuffer, *globalState.Pipeline, _drawUniforms);

				if (drawSet)
				{
					// Bind the instance descriptor set
					vkCmdBindDescriptorSets(
						cmdBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						globalState.Pipeline->GetPipelineLayout(),
						3,
						1, &drawSet,
						0, 0);
				}
			}
		}
		catch (const std::exception& ex)
		{
			CocoError("Error flushing state changes: {}", ex.what())
			stateBound = false;
		}

		_vulkanRenderOperation->StateChanges.clear();
		return stateBound;
	}

	VulkanShader& VulkanRenderContext::GetBoundShader()
	{
		Assert(_vulkanRenderOperation.has_value())
		Assert(_vulkanRenderOperation->GlobalState.has_value())

		auto& resources = Engine::Get()->GetResourceLibrary();

		SharedRef<Shader> shader;
		if (!resources.TryFindByName(_vulkanRenderOperation->GlobalState->ShaderName, shader))
		{
			shader = RenderService::Get()->GetErrorShader();
		}

		Assert(shader)

		VulkanShaderCache& shaderCache = static_cast<VulkanShaderCache&>(_device.GetShaderCache());
		return shaderCache.GetOrCreateShader(shader);
	}

	VulkanShaderUniformData& VulkanRenderContext::GetUniformDataForShader(const VulkanShader& shader)
	{
		VulkanRenderContextCache& cache = _deviceCache.GetOrCreateContextCache(ID);
		return cache.GetOrCreateShaderUniformData(shader);
	}
}