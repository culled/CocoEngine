#include "RenderContextVulkan.h"

#include <Coco/Core/Logging/Logger.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include <Coco/Rendering/RenderingService.h>
#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Material.h>
#include <Coco/Rendering/Shader.h>
#include "../VulkanUtilities.h"
#include "../GraphicsDeviceVulkan.h"
#include "../GraphicsPlatformVulkan.h"
#include "Cache/VulkanShader.h"
#include "Cache/VulkanRenderPass.h"
#include "Cache/VulkanPipeline.h"
#include "Cache/VulkanFramebuffer.h"
#include "Cache/VulkanShaderResource.h"
#include "../VulkanRenderCache.h"
#include "ImageVulkan.h"
#include "ImageSamplerVulkan.h"
#include "CommandBufferVulkan.h"
#include "VulkanDescriptorPool.h"
#include "BufferVulkan.h"
#include "../CommandBufferPoolVulkan.h"

namespace Coco::Rendering::Vulkan
{
	RenderContextVulkan::RenderContextVulkan(const ResourceID& id, const string& name) :
		GraphicsResource<GraphicsDeviceVulkan, RenderContext>(id, name), 
		_currentState(RenderContextState::Ready)
	{
		if (!_device->GetGraphicsCommandPool(_pool))
			throw VulkanRenderingException("A graphics command pool needs to be created for rendering");

		_commandBuffer = _pool->Allocate(true);

		_globalUBO = _device->CreateResource<BufferVulkan>(FormattedString("{} Global UBO", name),
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			sizeof(GlobalUniformObject),
			true);

		_imageAvailableSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>(FormattedString("{} Image Available Semaphore", name));
		_renderingCompleteSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>(FormattedString("{} Rendering Complete Semaphore", name));
		_renderingCompleteFence = _device->CreateResource<GraphicsFenceVulkan>(FormattedString("{} Rendering Complete Fence", name), true);

		_renderCache = CreateManagedRef<RenderContextVulkanCache>();

		CreateGlobalDescriptorSet();
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_device->WaitForIdle();

		_device->PurgeResource(_globalDescriptorPool);
		vkDestroyDescriptorSetLayout(_device->GetDevice(), _globalDescriptor.Layout, nullptr);
		_globalDescriptor.Layout = nullptr;

		_renderCache.Reset();
		_currentFramebuffer = nullptr;

		_frameSignalSemaphores.Clear();
		_frameWaitSemaphores.Clear();

		_pool->Free(_commandBuffer);
		_commandBuffer = Ref<CommandBufferVulkan>();

		_device->PurgeResource(_imageAvailableSemaphore);
		_device->PurgeResource(_renderingCompleteSemaphore);
		_device->PurgeResource(_renderingCompleteFence);
		_device->PurgeResource(_globalUBO);
	}

	void RenderContextVulkan::SetViewport(const RectInt& rect)
	{
		VkViewport viewport = {};
		viewport.x = static_cast<float>(rect.Offset.X);
		viewport.y = static_cast<float>(-rect.Offset.Y);
		viewport.width = static_cast<float>(rect.Size.Width);
		viewport.height = static_cast<float>(rect.Size.Height);
		viewport.minDepth = 0.0f; // TODO: configurable min/max depth
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(_commandBuffer->GetCmdBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = static_cast<uint32_t>(rect.Size.Width);
		scissor.extent.height = static_cast<uint32_t>(rect.Size.Height);

		vkCmdSetScissor(_commandBuffer->GetCmdBuffer(), 0, 1, &scissor);
	}

	void RenderContextVulkan::Draw(const ObjectRenderData& objectData)
	{
		MeshRenderData& meshData = _currentRenderView->Meshs.at(objectData.MeshData);

		// Sanity checks
		if (!meshData.VertexBuffer.IsValid() || !meshData.IndexBuffer.IsValid())
		{
			LogError(_device->GetLogger(), "Mesh has no index and/or vertex buffer. Skipping...");
			return;
		}

		if (meshData.VertexCount == 0 || meshData.IndexCount == 0)
		{
			LogError(_device->GetLogger(), "Mesh has no index and/or vertex data. Skipping...");
			return;
		}

		// Bind the object's material if it has one
		if(objectData.MaterialData != Resource::InvalidID)
			UseMaterial(objectData.MaterialData);

		// Flush the pipeline state
		if (!FlushStateChanges())
		{
			LogError(_device->GetLogger(), "Failed setting up state for rendering mesh. Skipping...");
			return;
		}

		// Bind the vertex buffer
		Array<VkDeviceSize, 1> offsets = { 0 };
		BufferVulkan* vertexBuffer = static_cast<BufferVulkan*>(meshData.VertexBuffer.Get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets.data());

		// Bind the index buffer
		BufferVulkan* indexBuffer = static_cast<BufferVulkan*>(meshData.IndexBuffer.Get());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Push the model matrix
		Array<float, Matrix4x4::CellCount> modelMat = objectData.ModelMatrix.AsFloat();
		vkCmdPushConstants(_commandBuffer->GetCmdBuffer(), _currentPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * Matrix4x4::CellCount, modelMat.data());

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), static_cast<uint>(meshData.IndexCount), 1, 0, 0, 0);

		_currentTrianglesDrawn += meshData.IndexCount / 3;
	}

	void RenderContextVulkan::WaitForRenderingCompleted()
	{
		_renderingCompleteFence->Wait(Math::MaxValue<uint64_t>());
	}

	bool RenderContextVulkan::BeginImpl()
	{
		try
		{
			if (!_currentRenderPipeline)
				throw VulkanRenderingException("No RenderPipline was set");

			_currentVulkanRenderPass = _device->GetRenderCache()->GetOrCreateRenderPass(_currentRenderPipeline);

			_globalUBO->LoadData(0, sizeof(GlobalUniformObject), &_globalUO);

			_currentFramebuffer = _renderCache->GetOrCreateFramebuffer(_currentRenderView, *_currentVulkanRenderPass, _currentRenderPipeline);

			_commandBuffer->Begin(true, false);

			AddPreRenderPassImageTransitions();

			VkRenderPassBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = _currentVulkanRenderPass->GetRenderPass();
			beginInfo.framebuffer = _currentFramebuffer->GetFramebuffer();

			beginInfo.renderArea.offset.x =	static_cast<uint32_t>(_currentRenderView->ViewportRect.Offset.X);
			beginInfo.renderArea.offset.y =	static_cast<uint32_t>(-_currentRenderView->ViewportRect.Offset.Y);
			beginInfo.renderArea.extent.width = static_cast<uint32_t>(_currentRenderView->ViewportRect.Size.Width);
			beginInfo.renderArea.extent.height = static_cast<uint32_t>(_currentRenderView->ViewportRect.Size.Height);

			List<VkClearValue> clearValues(_currentRenderView->RenderTargets.Count());
			const VkClearDepthStencilValue depthStencilClearValue{ 1.0f, 0 };

			// Set clear clear color for each render target
			for (int i = 0; i < clearValues.Count(); i++)
			{
				const RenderTarget& rt = _currentRenderView->RenderTargets[i];
				if (IsDepthStencilFormat(rt.Image->GetDescription().PixelFormat))
				{
					clearValues[i].depthStencil = depthStencilClearValue;
				}
				else
				{
					clearValues[i].color = 
					{ 
						static_cast<float>(rt.ClearColor.R), 
						static_cast<float>(rt.ClearColor.G), 
						static_cast<float>(rt.ClearColor.B),
						static_cast<float>(rt.ClearColor.A) 
					};
				}				
			}

			beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.Count());
			beginInfo.pClearValues = clearValues.Data();

			vkCmdBeginRenderPass(_commandBuffer->GetCmdBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

			_currentState = RenderContextState::CompilingDrawCalls;

			return true;
		}
		catch (const RenderingException& ex)
		{
			LogError(_device->GetLogger(), FormattedString("Unable to begin VulkanRenderContext: {}", ex.what()));
			return false;
		}
	}

	bool RenderContextVulkan::BeginPassImpl()
	{
		if (_currentRenderPassIndex > 0)
		{
			vkCmdNextSubpass(_commandBuffer->GetCmdBuffer(), VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		return true;
	}

	void RenderContextVulkan::EndImpl()
	{
		try
		{
			// Flush uniform data changes
			_renderCache->FlushUniformBufferChanges();

			List<GraphicsSemaphore*> waitSemaphores;

			for (Ref<GraphicsSemaphoreVulkan>& semaphore : _frameWaitSemaphores)
				if(semaphore.IsValid())
					waitSemaphores.Add(semaphore.Get());

			List<GraphicsSemaphore*> signalSemaphores;

			for (Ref<GraphicsSemaphoreVulkan>& semaphore : _frameSignalSemaphores)
				if (semaphore.IsValid())
					signalSemaphores.Add(semaphore.Get());

			vkCmdEndRenderPass(_commandBuffer->GetCmdBuffer());

			AddPostRenderPassImageTransitions();

			_commandBuffer->EndAndSubmit(&waitSemaphores, &signalSemaphores, _renderingCompleteFence.Get());

			_currentState = RenderContextState::DrawCallsSubmitted;
		}
		catch(const RenderingException& ex)
		{
			LogError(_device->GetLogger(), FormattedString("Unable to end VulkanRenderContext: {}", ex.what()));
		}

		_currentShaderUniformData = ShaderUniformData();
		_currentPipeline = nullptr;
		_currentVulkanRenderPass = nullptr;
	}

	void RenderContextVulkan::ResetImpl()
	{
		// Free all material descriptor sets
		_renderCache->FreeDescriptorSets();
		_shaderDescriptorSets.clear();

		// Clear all currently bound items
		_stateChanges.clear();
		_currentPipeline = nullptr;

		// Reset render syncronization objects
		_frameSignalSemaphores.Clear();
		_frameWaitSemaphores.Clear();

		_frameSignalSemaphores.Add(_renderingCompleteSemaphore);
		_frameWaitSemaphores.Add(_imageAvailableSemaphore);
		_renderingCompleteFence->Reset();

		_currentState = RenderContextState::Ready;
	}

	void RenderContextVulkan::NewShaderBound()
	{
		_stateChanges.insert(RenderContextStateChange::Shader);
		_currentPipeline = nullptr;
	}

	void RenderContextVulkan::NewMaterialBound()
	{
		_stateChanges.insert(RenderContextStateChange::Material);
	}

	bool RenderContextVulkan::FlushStateChanges()
	{
		bool stateBound = false;

		try
		{
			if (!_currentVulkanRenderPass)
				throw RenderingException("A render pass was not set");

			// We need to have a shader bound to draw anything
			if (_currentShader.ID == Resource::InvalidID)
				throw RenderingException("No shader was bound");

			VulkanShader* shader = _device->GetRenderCache()->GetOrCreateVulkanShader(_currentShader);

			if (_stateChanges.contains(RenderContextStateChange::Shader))
			{
				_currentPipeline = _device->GetRenderCache()->GetOrCreatePipeline(
					*_currentVulkanRenderPass,
					*shader,
					_currentRenderPass->GetSubshaderName(),
					static_cast<uint32_t>(_currentRenderPassIndex),
					_globalDescriptor.Layout);

				vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _currentPipeline->GetPipeline());

				// Bind the global descriptor set
				vkCmdBindDescriptorSets(
					_commandBuffer->GetCmdBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					_currentPipeline->GetPipelineLayout(),
					_globalDescriptorSetIndex, 1, &_globalDescriptorSet,
					0, 0);
			}

			VkDescriptorSet set;

			if (GetOrAllocateShaderDescriptorSet(*shader, set))
			{
				// Bind the material descriptor set
				vkCmdBindDescriptorSets(
					_commandBuffer->GetCmdBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					_currentPipeline->GetPipelineLayout(),
					_materialDescriptorSetIndex, 1, &set,
					0, 0);
			}

			// Ensure a pipeline state has been bound
			if(_currentPipeline == nullptr)
				throw RenderingException("A pipeline was not bound");

			stateBound = true;
			_currentDrawCallCount++;
		}
		catch (const RenderingException& ex)
		{
			LogError(_device->GetLogger(), FormattedString("Failed binding pipeline state: {}", ex.what()));
			stateBound = false;
		}

		_stateChanges.clear();
		return stateBound;
	}

	void RenderContextVulkan::CreateGlobalDescriptorSet()
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		_globalDescriptor.LayoutBindings.Add(binding);

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = 1;
		layoutCreateInfo.pBindings = &binding;

		AssertVkResult(vkCreateDescriptorSetLayout(_device->GetDevice(), &layoutCreateInfo, nullptr, &_globalDescriptor.Layout));

		_globalDescriptorPool = _device->CreateResource<VulkanDescriptorPool>(FormattedString("{} Global Descriptor Pool", _name), 1, List<VulkanDescriptorLayout>({ _globalDescriptor }));
		_globalDescriptorSet = _globalDescriptorPool->GetOrAllocateSet(_globalDescriptor, 0);

		// Update the descriptor sets
		VkDescriptorBufferInfo globalUBOBufferInfo = {};
		globalUBOBufferInfo.buffer = _globalUBO->GetBuffer();
		globalUBOBufferInfo.offset = 0;
		globalUBOBufferInfo.range = sizeof(GlobalUniformObject);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _globalDescriptorSet;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &globalUBOBufferInfo;

		vkUpdateDescriptorSets(_device->GetDevice(), 1, &descriptorWrite, 0, nullptr);
	}

	bool RenderContextVulkan::GetOrAllocateShaderDescriptorSet(const VulkanShader& shader, VkDescriptorSet& set)
	{
		// Use the cached descriptor set if it exists
		const auto it = _shaderDescriptorSets.find(_currentShaderUniformData.ID);
		if (it != _shaderDescriptorSets.cend())
		{
			set = it->second;
			return true;
		}

		const string renderPassName = _currentRenderPass->GetSubshaderName();
		const VulkanSubshader* subshader;
		if (!shader.TryGetSubshader(renderPassName, subshader))
			throw RenderingException(FormattedString("Shader {} has no subshader named {}", shader.GetName(), renderPassName));

		const Subshader& subshaderInfo = subshader->GetSubshader();

		// Early out if no descriptors
		if (subshaderInfo.Descriptors.Count() == 0 && subshaderInfo.Samplers.Count() == 0)
			return false;

		// Get the shader resource
		VulkanShaderResource* shaderResource = _renderCache->GetOrCreateShaderResource(shader);

		// Allocate this shader's descriptor set
		auto[region, descriptorSet] = shaderResource->CreateOrUpdateBuffer(_currentShaderUniformData, *subshader);
		_shaderDescriptorSets[_currentShaderUniformData.ID] = descriptorSet;
		set = descriptorSet;

		bool hasUniforms = subshaderInfo.Descriptors.Count() > 0;

		VkDescriptorBufferInfo bufferInfo = {};
		List<VkDescriptorImageInfo> imageInfos(subshaderInfo.Samplers.Count());
		List<VkWriteDescriptorSet> descriptorWrites(imageInfos.Count() + (hasUniforms ? 1 : 0));

		uint32_t bindingIndex = 0;

		// Write buffer binding if we need it
		if (hasUniforms)
		{
			VkWriteDescriptorSet& write = descriptorWrites[bindingIndex];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = set;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.descriptorCount = 1;

			bufferInfo.buffer = shaderResource->GetUniformBuffer(region)->GetBuffer();
			bufferInfo.offset = region.Offset;
			bufferInfo.range = region.Length;

			write.pBufferInfo = &bufferInfo;

			bindingIndex++;
		}

		// Write texture bindings
		for (uint32_t i = 0; i < subshaderInfo.Samplers.Count(); i++)
		{
			const ShaderTextureSampler& textureSampler = subshaderInfo.Samplers[i];

			ResourceID textureID = _currentShaderUniformData.Textures.contains(textureSampler.Name) ? _currentShaderUniformData.Textures.at(textureSampler.Name) : Resource::InvalidID;

			Ref<Image> image;
			Ref<ImageSampler> sampler;

			if (textureID != Resource::InvalidID)
			{
				const TextureRenderData& textureData = _currentRenderView->Textures.at(textureID);
				image = textureData.Image;
				sampler = textureData.Sampler;
			}
			else
			{
				Ref<Texture> defaultTexture = EnsureRenderingService()->GetDefaultCheckerTexture();
				image = defaultTexture->GetImage();
				sampler = defaultTexture->GetSampler();
			}

			if (!image.IsValid() || !sampler.IsValid())
				throw RenderingException(FormattedString("The sampler or image reference for \"{}\" is empty", textureSampler.Name));

			ImageVulkan* vulkanImage = static_cast<ImageVulkan*>(image.Get());
			ImageSamplerVulkan* vulkanSampler = static_cast<ImageSamplerVulkan*>(sampler.Get());

			// Texture data
			VkDescriptorImageInfo& imageInfo = imageInfos[i];
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanImage->GetNativeView();
			imageInfo.sampler = vulkanSampler->GetSampler();

			VkWriteDescriptorSet& write = descriptorWrites[bindingIndex];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = set;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			write.descriptorCount = 1;
			write.pImageInfo = &imageInfo;

			bindingIndex++;
		}	

		vkUpdateDescriptorSets(_device->GetDevice(), static_cast<uint32_t>(descriptorWrites.Count()), descriptorWrites.Data(), 0, nullptr);

		return true;
	}

	void RenderContextVulkan::AddPreRenderPassImageTransitions()
	{
		const auto& pipelineAttachments = _currentRenderPipeline->GetPipelineAttachmentDescriptions();

		for (uint64_t i = 0; i < _currentRenderView->RenderTargets.Count(); i++)
		{
			// Don't bother transitioning layouts for attachments that aren't preserved
			if (!pipelineAttachments[i].Description.ShouldPreserve)
				continue;

			Ref<Image>& rt = _currentRenderView->RenderTargets[i].Image;

			VkImageLayout layout = ToAttachmentLayout(pipelineAttachments[i].Description.PixelFormat);

			ImageVulkan* image = static_cast<ImageVulkan*>(rt.Get());
			image->TransitionLayout(
				*_commandBuffer.Get(),
				layout
			);
		}
	}

	void RenderContextVulkan::AddPostRenderPassImageTransitions()
	{
		const auto& pipelineAttachments = _currentRenderPipeline->GetPipelineAttachmentDescriptions();

		for (uint64_t i = 0; i < _currentRenderView->RenderTargets.Count(); i++)
		{
			Ref<Image>& rt = _currentRenderView->RenderTargets[i].Image;

			VkImageLayout layout = ToAttachmentLayout(pipelineAttachments[i].Description.PixelFormat);

			ImageVulkan* image = static_cast<ImageVulkan*>(rt.Get());
			image->SetCurrentLayout(layout);

			if ((rt->GetDescription().UsageFlags & ImageUsageFlags::Presented) == ImageUsageFlags::Presented)
			{
				image->TransitionLayout(
					*_commandBuffer.Get(),  
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				);
			}
		}
	}
}