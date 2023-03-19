#include "RenderContextVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include "ImageVulkan.h"
#include <Coco/Core/Logging/Logger.h>
#include "GraphicsPlatformVulkan.h"
#include "BufferVulkan.h"
#include "VulkanShader.h"
#include "ImageSamplerVulkan.h"
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Rendering/RenderingService.h>

#include "VulkanUtilities.h"

namespace Coco::Rendering::Vulkan
{
	RenderContextVulkan::RenderContextVulkan(GraphicsDeviceVulkan* device) :
		_device(device), _currentState(RenderContextState::Ready)
	{
		_renderingService = RenderingService::Get();
		if (_renderingService == nullptr)
			throw RenderingException("Could not find an active rendering service");

		if (!_device->GetGraphicsCommandPool(_pool))
			throw VulkanRenderingException("A graphics command pool needs to be created for rendering");

		_commandBuffer = static_cast<CommandBufferVulkan*>(_pool->Allocate(true));

		_globalUBO = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			sizeof(GlobalUniformObject),
			true);

		_materialUBO = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			s_materialBufferSize,
			true);

		_imageAvailableSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteFence = _device->CreateResource<GraphicsFenceVulkan>(true);

		_renderCache = CreateManaged<RenderContextVulkanCache>(_device);

		CreateGlobalDescriptorSet();
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_device->WaitForIdle();

		_globalDescriptorPool.reset();
		vkDestroyDescriptorSetLayout(_device->GetDevice(), _globalDescriptor.Layout, nullptr);
		_globalDescriptor.Layout = nullptr;

		_renderCache.reset();

		_signalSemaphores.Clear();
		_waitSemaphores.Clear();
		_renderTargets.Clear();

		_pool->Free(_commandBuffer);
		_commandBuffer = nullptr;
		_pool = nullptr;

		_imageAvailableSemaphore.reset();
		_renderingCompleteSemaphore.reset();
		_renderingCompleteFence.reset();
		_globalUBO.reset();
		_materialUBO.reset();

		DestroyFramebuffer();
	}

	void RenderContextVulkan::SetViewport(const Vector2Int& offset, const SizeInt& size)
	{
		VkViewport viewport = {};
		viewport.x = static_cast<float>(offset.X);
		viewport.y = static_cast<float>(offset.Y);
		viewport.width = static_cast<float>(size.Width);
		viewport.height = static_cast<float>(size.Height);
		viewport.minDepth = 0.0f; // TODO: configurable min/max depth
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(_commandBuffer->GetCmdBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = static_cast<uint32_t>(size.Width);
		scissor.extent.height = static_cast<uint32_t>(size.Height);

		vkCmdSetScissor(_commandBuffer->GetCmdBuffer(), 0, 1, &scissor);
	}

	void RenderContextVulkan::UseShader(Ref<Shader> shader)
	{
		// No need to change if the same shader is used
		if (shader.get() == _currentShader.get())
			return;

		_stateChanges.insert(RenderContextStateChange::Shader);
		_currentShader = shader;

		// Reset the pipeline and material as we're using a different shader
		_currentPipeline = VulkanPipeline::None;
		_currentMaterial = nullptr;
	}

	void RenderContextVulkan::UseMaterial(Ref<Material> material)
	{
		// No need to change if the same material is used
		if (material.get() == _currentMaterial.get())
			return;

		// Bind the material shader
		if (material.get() != nullptr)
			UseShader(material->GetShader());
		else
			UseShader(nullptr);

		_stateChanges.insert(RenderContextStateChange::Material);
		_currentMaterial = material;
	}

	void RenderContextVulkan::SetRenderTargets(const List<GraphicsResourceRef<ImageVulkan>>& renderTargets)
	{
		DestroyFramebuffer();
		_renderTargets = renderTargets;
	}

	void RenderContextVulkan::Draw(const Mesh* mesh, const Matrix4x4& modelMatrix)
	{
		// Sanity checks
		GraphicsResourceRef<Buffer> vertexBufferRef = mesh->GetVertexBuffer();
		GraphicsResourceRef<Buffer> indexBufferRef = mesh->GetIndexBuffer();
		if (vertexBufferRef == nullptr || indexBufferRef == nullptr)
		{
			LogError(_renderingService->GetLogger(), "Mesh has no index and/or vertex buffer. Skipping...");
			return;
		}

		if (vertexBufferRef->GetSize() == 0 || indexBufferRef->GetSize() == 0)
		{
			LogError(_renderingService->GetLogger(), "Mesh has no index and/or vertex data. Skipping...");
			return;
		}

		// Flush the pipeline state
		if (!FlushStateChanges())
		{
			LogError(_renderingService->GetLogger(), "Failed setting up state for rendering mesh. Skipping...");
			return;
		}

		// Bind the vertex buffer
		Array<VkDeviceSize, 1> offsets = { 0 };
		const BufferVulkan* vertexBuffer = static_cast<BufferVulkan*>(vertexBufferRef.get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets.data());

		// Bind the index buffer
		const BufferVulkan* indexBuffer = static_cast<BufferVulkan*>(indexBufferRef.get());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Push the model matrix
		Array<float, Matrix4x4::CellCount> modelMat = modelMatrix.AsFloat();
		vkCmdPushConstants(_commandBuffer->GetCmdBuffer(), _currentPipeline.Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * Matrix4x4::CellCount, modelMat.data());

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), static_cast<uint>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}

	void RenderContextVulkan::WaitForRenderingCompleted()
	{
		_renderingCompleteFence->Wait(Math::MaxValue<uint64_t>());
	}

	bool RenderContextVulkan::BeginImpl()
	{
		try
		{
			_renderPass = _device->GetRenderCache()->GetOrCreateRenderPass(CurrentPipeline.get());

			_globalUBO->LoadData(0, sizeof(GlobalUniformObject), &GlobalUO);

			if (_framebuffer == nullptr)
				CreateFramebuffer();

			_commandBuffer->Begin(true, false);

			VkRenderPassBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = _renderPass.RenderPass;
			beginInfo.framebuffer = _framebuffer;

			beginInfo.renderArea.offset.x = static_cast<uint32_t>(RenderView->RenderOffset.X);
			beginInfo.renderArea.offset.y = static_cast<uint32_t>(RenderView->RenderOffset.Y);
			beginInfo.renderArea.extent.width = static_cast<uint32_t>(RenderView->RenderSize.Width);
			beginInfo.renderArea.extent.height = static_cast<uint32_t>(RenderView->RenderSize.Height);

			const VkClearValue clearValue = { {{
					static_cast<float>(RenderView->ClearColor.R),
					static_cast<float>(RenderView->ClearColor.G),
					static_cast<float>(RenderView->ClearColor.B),
					static_cast<float>(RenderView->ClearColor.A)
				}} };

			beginInfo.clearValueCount = 1;
			beginInfo.pClearValues = &clearValue;

			vkCmdBeginRenderPass(_commandBuffer->GetCmdBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

			_currentState = RenderContextState::CompilingDrawCalls;

			return true;
		}
		catch (const RenderingException& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to begin VulkanRenderContext: {}", ex.what()));
			return false;
		}
	}

	void RenderContextVulkan::EndImpl()
	{
		try
		{
			// Flush material data changes
			if (_mappedMaterialUBOMemory != nullptr)
			{
				_materialUBO->Unlock();
				_mappedMaterialUBOMemory = nullptr;
			}

			List<IGraphicsSemaphore*> waitSemaphores;

			for (GraphicsResourceRef<GraphicsSemaphoreVulkan> semaphore : _waitSemaphores)
				waitSemaphores.Add(semaphore.get());

			List<IGraphicsSemaphore*> signalSemaphores;

			for (GraphicsResourceRef<GraphicsSemaphoreVulkan> semaphore : _signalSemaphores)
				signalSemaphores.Add(semaphore.get());

			vkCmdEndRenderPass(_commandBuffer->GetCmdBuffer());
			_commandBuffer->EndAndSubmit(waitSemaphores, signalSemaphores, _renderingCompleteFence.get());

			_currentState = RenderContextState::DrawCallsSubmitted;
		}
		catch(const RenderingException& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to end VulkanRenderContext: {}", ex.what()));
		}
	}

	void RenderContextVulkan::ResetImpl()
	{
		// Free all material descriptor sets
		_renderCache->FreeDescriptorSets();
		_materialDescriptorSets.clear();

		// TODO: find a better way to handle too much material data
		if (_currentMaterialUBOOffset == Math::MaxValue<uint64_t>())
			_currentMaterialUBOOffset = 0;

		// Clear all currently bound items
		_stateChanges.clear();
		_currentShader.reset();
		_currentMaterial.reset();
		_currentPipeline = VulkanPipeline::None;

		// Reset render syncronization objects
		_signalSemaphores.Clear();
		_waitSemaphores.Clear();

		_waitSemaphores.Add(_imageAvailableSemaphore);
		_signalSemaphores.Add(_renderingCompleteSemaphore);
		_renderingCompleteFence->Reset();

		_currentState = RenderContextState::Ready;
	}

	bool RenderContextVulkan::FlushStateChanges()
	{
		bool stateBound = false;

		try
		{
			if (CurrentRenderPass == nullptr)
				throw RenderingException("A render pass was not set");

			// We need to have a shader bound to draw anything
			if (_currentShader.get() == nullptr)
				throw RenderingException("No shader was bound");

			if (_stateChanges.contains(RenderContextStateChange::Shader))
			{
				_currentPipeline = _device->GetRenderCache()->GetOrCreatePipeline(
					_renderPass,
					CurrentRenderPass->GetName(),
					CurrentRenderPassIndex,
					_currentShader.get(),
					_globalDescriptor.Layout);

				vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _currentPipeline.Pipeline);

				// Bind the global descriptor set
				vkCmdBindDescriptorSets(
					_commandBuffer->GetCmdBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					_currentPipeline.Layout,
					s_globalDescriptorSetIndex, 1, &_globalDescriptorSet,
					0, 0);
			}

			// Rendering without a material is fine
			if (_stateChanges.contains(RenderContextStateChange::Material) && _currentMaterial.get() != nullptr)
			{
				VkDescriptorSet set;
				
				if (GetOrAllocateMaterialDescriptorSet(set))
				{
					// Bind the material descriptor set
					vkCmdBindDescriptorSets(
						_commandBuffer->GetCmdBuffer(),
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						_currentPipeline.Layout,
						s_materialDescriptorSetIndex, 1, &set,
						0, 0);
				}
			}

			// Ensure a pipeline state has been bound
			if(_currentPipeline == VulkanPipeline::None)
				throw RenderingException("A pipeline was not bound");

			stateBound = true;
		}
		catch (const RenderingException& ex)
		{
			_currentPipeline = VulkanPipeline::None;
			_currentShader = nullptr;
			_currentMaterial = nullptr;

			LogError(_renderingService->GetLogger(), FormattedString("Failed binding pipeline state: {}", ex.what()));
			stateBound = false;
		}

		_stateChanges.clear();
		return stateBound;
	}

	void RenderContextVulkan::CreateFramebuffer()
	{
		_framebufferPipeline = CurrentPipeline;

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _renderPass.RenderPass;
		framebufferInfo.width = static_cast<uint32_t>(RenderView->RenderSize.Width);
		framebufferInfo.height = static_cast<uint32_t>(RenderView->RenderSize.Height);
		framebufferInfo.layers = 1;

		List<VkImageView> renderTargets;
		for (const GraphicsResourceRef<ImageVulkan> renderTarget : _renderTargets)
		{
			renderTargets.Add(renderTarget->GetNativeView());
		}

		framebufferInfo.attachmentCount = static_cast<uint32_t>(renderTargets.Count());
		framebufferInfo.pAttachments = renderTargets.Data();

		AssertVkResult(vkCreateFramebuffer(_device->GetDevice(), &framebufferInfo, nullptr, &_framebuffer));
	}

	void RenderContextVulkan::DestroyFramebuffer() noexcept
	{
		_device->WaitForIdle();

		vkDestroyFramebuffer(_device->GetDevice(), _framebuffer, nullptr);
		_framebuffer = nullptr;
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

		_globalDescriptorPool = _device->CreateResource<VulkanDescriptorPool>(1, List<VulkanDescriptorLayout>({ _globalDescriptor }));
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

	bool RenderContextVulkan::GetOrAllocateMaterialDescriptorSet(VkDescriptorSet& set)
	{
		// Get the subshader
		const string subshaderName = CurrentRenderPass->GetName();
		const Subshader* subshader;
		if (!_currentShader->TryGetSubshader(subshaderName, subshader))
			throw RenderingException(FormattedString("Could not find a subshader in \"{}\" for pass {}",
				_currentShader->GetName(),
				subshaderName,
				_currentShader->GetName()
			));

		// Early out if no descriptors
		if (subshader->Descriptors.Count() == 0 && subshader->Samplers.Count() == 0)
			return false;

		CachedMaterialResource& materialResource = _renderCache->GetMaterialResource(_currentMaterial.get());

		// Update the material uniform data
		if (materialResource.NeedsUpdate(_currentMaterial.get()) && _currentMaterialUBOOffset != Math::MaxValue<uint64_t>())
		{
			// Map the buffer data if it isn't already
			if(_mappedMaterialUBOMemory == nullptr)
				_mappedMaterialUBOMemory = _materialUBO->Lock(0, s_materialBufferSize);

			// Tell the resource to update. It will use its current offset if it still fits in its block of the buffer
			bool materialDataAppended = false;

			try
			{
				materialResource.Update(_device, _currentMaterial.get(), _mappedMaterialUBOMemory, _currentMaterialUBOOffset, _materialUBO->GetSize(), materialDataAppended);
			}
			catch (const VulkanRenderingException& ex)
			{
				LogError(_renderingService->GetLogger(), ex.what());
				_currentMaterialUBOOffset = Math::MaxValue<uint64_t>();
				return false;
			}

			// The material data was appended to the buffer, so update the current offset
			if(materialDataAppended)
				_currentMaterialUBOOffset = materialResource.BufferOffset + materialResource.BufferSize;
		}

		const ResourceID materialID = _currentMaterial->GetID();

		// Use the cached descriptor set if it exists
		if (_materialDescriptorSets.contains(materialID))
			return _materialDescriptorSets.at(materialID);

		// Get the vulkan shader and this subshader's descriptor layout
		GraphicsResourceRef<VulkanShader> vulkanShader = _device->GetRenderCache()->GetOrCreateVulkanShader(_currentShader.get());
		VulkanDescriptorLayout layout;
		if (!vulkanShader->TryGetDescriptorSetLayout(subshaderName, layout))
			throw RenderingException(FormattedString("Could not find a descriptor layout for subshader \"{}\" in shader \"{}\"",
				subshaderName,
				_currentShader->GetName()
			));

		// Get the shader resource
		CachedShaderResource& shaderResource = _renderCache->GetShaderResource(vulkanShader.get());
		if (shaderResource.NeedsUpdate(vulkanShader.get()))
		{
			shaderResource.Update(_device, vulkanShader.get());
		}

		// Allocate this material's descriptor set
		set = shaderResource.Pool->GetOrAllocateSet(layout, materialID);
		_materialDescriptorSets[materialID] = set;

		// Get the material's binding for this subshader
		SubshaderUniformBinding* subshaderBinding = nullptr;
		_currentMaterial->TryGetSubshaderBinding(subshaderName, subshaderBinding);

		VkDescriptorBufferInfo bufferInfo = {};
		List<VkDescriptorImageInfo> imageInfos(subshader->Samplers.Count());
		List<VkWriteDescriptorSet> descriptorWrites(imageInfos.Count() + (subshaderBinding != nullptr ? 1 : 0));

		uint32_t bindingIndex = 0;

		// Write buffer binding (if it exists)
		if (subshaderBinding != nullptr)
		{
			VkWriteDescriptorSet& write = descriptorWrites[bindingIndex];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = set;
			write.dstBinding = bindingIndex;
			write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			write.descriptorCount = 1;

			bufferInfo.buffer = _materialUBO->GetBuffer();
			bufferInfo.offset = materialResource.BufferOffset + subshaderBinding->Offset;
			bufferInfo.range = subshaderBinding->Size;

			write.pBufferInfo = &bufferInfo;

			bindingIndex++;
		}

		// Write texture bindings
		for (uint32_t i = 0; i < subshader->Samplers.Count(); i++)
		{
			const ShaderTextureSampler& textureSampler = subshader->Samplers[i];

			Ref<Texture> texture = _currentMaterial->GetTexture(textureSampler.Name);

			if (!texture)
				texture = _renderingService->GetDefaultCheckerTexture();

			GraphicsResourceRef<Image> image = texture->GetImage();
			const ImageVulkan* vulkanImage = static_cast<ImageVulkan*>(image.get());
			GraphicsResourceRef<ImageSampler> sampler = texture->GetSampler();
			const ImageSamplerVulkan* vulkanSampler = static_cast<ImageSamplerVulkan*>(sampler.get());

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
}