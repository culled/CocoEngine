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
#include "../VulkanShader.h"
#include "../VulkanRenderCache.h"
#include "ImageVulkan.h"
#include "ImageSamplerVulkan.h"
#include "CommandBufferVulkan.h"
#include "VulkanDescriptorPool.h"
#include "BufferVulkan.h"
#include "RenderContextVulkanCache.h"
#include "CommandBufferPoolVulkan.h"


namespace Coco::Rendering::Vulkan
{
	CachedVulkanFramebuffer::CachedVulkanFramebuffer(GraphicsDeviceVulkan* device, Ref<RenderPipeline> pipeline) : CachedResource(pipeline->GetID(), pipeline->GetVersion()),
		Device(device), PipelineRef(pipeline)
	{}

	CachedVulkanFramebuffer::~CachedVulkanFramebuffer()
	{
		DestroyFramebuffer();
	}

	bool CachedVulkanFramebuffer::NeedsUpdate() const noexcept
	{
		if (Ref<RenderPipeline> pipeline = PipelineRef.lock())
		{
			return Framebuffer == nullptr || FramebufferSize == SizeInt::Zero || this->Version != pipeline->GetVersion();
		}

		return false;
	}

	void CachedVulkanFramebuffer::DestroyFramebuffer() noexcept
	{
		if (Framebuffer != nullptr)
		{
			Device->WaitForIdle();
			vkDestroyFramebuffer(Device->GetDevice(), Framebuffer, nullptr);
		}

		Framebuffer = nullptr;
		FramebufferSize = SizeInt::Zero;
	}

	RenderContextVulkan::RenderContextVulkan(GraphicsDevice* device) :
		_device(static_cast<GraphicsDeviceVulkan*>(device)), _currentState(RenderContextState::Ready)
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

		_imageAvailableSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteFence = _device->CreateResource<GraphicsFenceVulkan>(true);

		_renderCache = CreateManaged<RenderContextVulkanCache>(_device);
		_device->OnPurgedResources.AddHandler(this, &RenderContextVulkan::HandlePurgeResources);

		CreateGlobalDescriptorSet();
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_device->OnPurgedResources.RemoveHandler(this, &RenderContextVulkan::HandlePurgeResources);

		_device->WaitForIdle();

		_globalDescriptorPool.Invalidate();
		vkDestroyDescriptorSetLayout(_device->GetDevice(), _globalDescriptor.Layout, nullptr);
		_globalDescriptor.Layout = nullptr;

		_renderCache.reset();
		_framebuffer.reset();

		_signalSemaphores.Clear();
		_waitSemaphores.Clear();
		_renderTargets.Clear();

		_pool->Free(_commandBuffer);
		_commandBuffer = nullptr;
		_pool = nullptr;

		_imageAvailableSemaphore.Invalidate();
		_renderingCompleteSemaphore.Invalidate();
		_renderingCompleteFence.Invalidate();
		_globalUBO.Invalidate();
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

	void RenderContextVulkan::UseShader(Ref<Shader> shader)
	{
		// No need to change if the same shader is used
		if (shader.get() == _currentShader.get())
			return;

		_stateChanges.insert(RenderContextStateChange::Shader);
		_currentShader = shader;

		// Reset the pipeline and material as we're using a different shader
		_currentPipeline = nullptr;
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

	void RenderContextVulkan::SetRenderTargets(const List<WeakManagedRef<ImageVulkan>>& renderTargets)
	{
		// TODO: Make a way to tell if the same render targets are used to prevent recreating framebuffer every frame
		if(_framebuffer)
			_framebuffer->DestroyFramebuffer();

		_renderTargets = renderTargets;
	}

	void RenderContextVulkan::Draw(const Mesh* mesh, const Matrix4x4& modelMatrix)
	{
		// Sanity checks
		WeakManagedRef<Buffer> vertexBufferRef = mesh->GetVertexBuffer();
		WeakManagedRef<Buffer> indexBufferRef = mesh->GetIndexBuffer();
		if (!vertexBufferRef.IsValid() || !indexBufferRef.IsValid())
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
		const BufferVulkan* vertexBuffer = static_cast<BufferVulkan*>(vertexBufferRef.Get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets.data());

		// Bind the index buffer
		const BufferVulkan* indexBuffer = static_cast<BufferVulkan*>(indexBufferRef.Get());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Push the model matrix
		Array<float, Matrix4x4::CellCount> modelMat = modelMatrix.AsFloat();
		vkCmdPushConstants(_commandBuffer->GetCmdBuffer(), _currentPipeline->Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * Matrix4x4::CellCount, modelMat.data());

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), static_cast<uint>(mesh->GetIndexCount()), 1, 0, 0, 0);

		TrianglesDrawn += mesh->GetIndexCount() / 3;
	}

	void RenderContextVulkan::WaitForRenderingCompleted()
	{
		_renderingCompleteFence->Wait(Math::MaxValue<uint64_t>());
	}

	bool RenderContextVulkan::BeginImpl()
	{
		try
		{
			if (CurrentPipeline == nullptr)
				throw VulkanRenderingException("No RenderPipline was set");

			_renderPass = _device->GetRenderCache()->GetOrCreateRenderPass(CurrentPipeline);

			_globalUBO->LoadData(0, sizeof(GlobalUniformObject), &GlobalUO);

			EnsureFramebufferUpdated();

			_commandBuffer->Begin(true, false);

			VkRenderPassBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = _renderPass->RenderPass;
			beginInfo.framebuffer = _framebuffer->Framebuffer;

			beginInfo.renderArea.offset.x =	static_cast<uint32_t>(RenderView->ViewportRect.Offset.X);
			beginInfo.renderArea.offset.y =	static_cast<uint32_t>(-RenderView->ViewportRect.Offset.Y);
			beginInfo.renderArea.extent.width = static_cast<uint32_t>(RenderView->ViewportRect.Size.Width);
			beginInfo.renderArea.extent.height = static_cast<uint32_t>(RenderView->ViewportRect.Size.Height);

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
			_renderCache->FlushMaterialChanges();

			List<IGraphicsSemaphore*> waitSemaphores;

			for (const WeakManagedRef<GraphicsSemaphoreVulkan>& semaphore : _waitSemaphores)
				if(semaphore.IsValid())
					waitSemaphores.Add(semaphore.Get());

			List<IGraphicsSemaphore*> signalSemaphores;

			for (const WeakManagedRef<GraphicsSemaphoreVulkan>& semaphore : _signalSemaphores)
				if (semaphore.IsValid())
					signalSemaphores.Add(semaphore.Get());

			vkCmdEndRenderPass(_commandBuffer->GetCmdBuffer());
			_commandBuffer->EndAndSubmit(waitSemaphores, signalSemaphores, _renderingCompleteFence.Get());

			_currentState = RenderContextState::DrawCallsSubmitted;
		}
		catch(const RenderingException& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to end VulkanRenderContext: {}", ex.what()));
		}

		_currentMaterial.reset();
		_currentShader.reset();
		_currentPipeline.reset();
	}

	void RenderContextVulkan::ResetImpl()
	{
		// Free all material descriptor sets
		_renderCache->FreeDescriptorSets();
		_materialDescriptorSets.clear();

		// Clear all currently bound items
		_stateChanges.clear();
		_currentShader.reset();
		_currentMaterial.reset();
		_currentPipeline = nullptr;

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
					*_renderPass,
					CurrentRenderPass->GetName(),
					CurrentRenderPassIndex,
					_currentShader,
					_globalDescriptor.Layout);

				vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _currentPipeline->Pipeline);

				// Bind the global descriptor set
				vkCmdBindDescriptorSets(
					_commandBuffer->GetCmdBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					_currentPipeline->Layout,
					s_globalDescriptorSetIndex, 1, &_globalDescriptorSet,
					0, 0);
			}

			// Rendering without a material is fine
			if (_stateChanges.contains(RenderContextStateChange::Material) && _currentMaterial.get() != nullptr)
			{
				VkDescriptorSet set;
				
				if (GetOrAllocateMaterialDescriptorSet(CurrentRenderPass->GetName(), _currentShader, _currentMaterial, set))
				{
					// Bind the material descriptor set
					vkCmdBindDescriptorSets(
						_commandBuffer->GetCmdBuffer(),
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						_currentPipeline->Layout,
						s_materialDescriptorSetIndex, 1, &set,
						0, 0);
				}
			}

			// Ensure a pipeline state has been bound
			if(_currentPipeline == nullptr)
				throw RenderingException("A pipeline was not bound");

			stateBound = true;
			DrawCallCount++;
		}
		catch (const RenderingException& ex)
		{
			_currentPipeline = nullptr;
			_currentShader = nullptr;
			_currentMaterial = nullptr;

			LogError(_renderingService->GetLogger(), FormattedString("Failed binding pipeline state: {}", ex.what()));
			stateBound = false;
		}

		_stateChanges.clear();
		return stateBound;
	}

	void RenderContextVulkan::EnsureFramebufferUpdated()
	{
		// Create the cached resource if necessary
		if (_framebuffer == nullptr || _framebuffer->IsInvalid())
			_framebuffer = CreateRef<CachedVulkanFramebuffer>(_device, CurrentPipeline);

		// Update the framebuffer if necessary
		if (_framebuffer->NeedsUpdate(RenderView->ViewportRect.Size))
		{
			_framebuffer->DestroyFramebuffer();

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass->RenderPass;
			framebufferInfo.width = static_cast<uint32_t>(RenderView->ViewportRect.Size.Width);
			framebufferInfo.height = static_cast<uint32_t>(RenderView->ViewportRect.Size.Height);
			framebufferInfo.layers = 1;

			List<VkImageView> renderTargets;
			for (const WeakManagedRef<ImageVulkan> renderTarget : _renderTargets)
			{
				if (!renderTarget.IsValid())
					throw VulkanRenderingException("Render target resource was invalid");

				renderTargets.Add(renderTarget->GetNativeView());
			}

			framebufferInfo.attachmentCount = static_cast<uint32_t>(renderTargets.Count());
			framebufferInfo.pAttachments = renderTargets.Data();

			AssertVkResult(vkCreateFramebuffer(_device->GetDevice(), &framebufferInfo, nullptr, &_framebuffer->Framebuffer));
			_framebuffer->FramebufferSize = RenderView->ViewportRect.Size;
		}
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

	bool RenderContextVulkan::GetOrAllocateMaterialDescriptorSet(const string& subshaderName, const Ref<Shader>& shader, const Ref<Material>& material, VkDescriptorSet& set)
	{
		// Get the subshader
		const Subshader* subshader;
		if (!shader->TryGetSubshader(subshaderName, subshader))
			throw RenderingException(FormattedString("Could not find a subshader in \"{}\" for pass {}",
				shader->GetName(),
				subshaderName
			));

		// Early out if no descriptors
		if (subshader->Descriptors.Count() == 0 && subshader->Samplers.Count() == 0)
			return false;

		CachedMaterialResource& materialResource = _renderCache->GetMaterialResource(material);
		const ResourceID materialID = material->GetID();

		// Use the cached descriptor set if it exists
		if (_materialDescriptorSets.contains(materialID))
			return _materialDescriptorSets.at(materialID);

		// Get the vulkan shader and this subshader's descriptor layout
		Ref<VulkanShader> vulkanShader = _device->GetRenderCache()->GetOrCreateVulkanShader(shader);
		VulkanDescriptorLayout layout;
		if (!vulkanShader->TryGetDescriptorSetLayout(subshaderName, layout))
			throw RenderingException(FormattedString("Could not find a descriptor layout for subshader \"{}\" in shader \"{}\"",
				subshaderName,
				shader->GetName()
			));

		// Get the shader resource
		CachedShaderResource& shaderResource = _renderCache->GetShaderResource(vulkanShader);

		// Allocate this material's descriptor set
		set = shaderResource.Pool->GetOrAllocateSet(layout, materialID);
		_materialDescriptorSets[materialID] = set;

		// Get the material's binding for this subshader
		SubshaderUniformBinding* subshaderBinding = nullptr;
		material->TryGetSubshaderBinding(subshaderName, subshaderBinding);

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

			bufferInfo.buffer = _renderCache->GetMaterialBuffer(materialResource.BufferIndex)->GetBuffer();
			bufferInfo.offset = materialResource.BufferOffset + subshaderBinding->Offset;
			bufferInfo.range = subshaderBinding->Size;

			write.pBufferInfo = &bufferInfo;

			bindingIndex++;
		}

		// Write texture bindings
		for (uint32_t i = 0; i < subshader->Samplers.Count(); i++)
		{
			const ShaderTextureSampler& textureSampler = subshader->Samplers[i];

			Ref<Texture> texture = material->GetTexture(textureSampler.Name);

			if (!texture)
				texture = _renderingService->GetDefaultCheckerTexture();

			WeakManagedRef<Image> image = texture->GetImage();
			WeakManagedRef<ImageSampler> sampler = texture->GetSampler();

			if (!image.IsValid() || !sampler.IsValid())
				throw RenderingException(FormattedString(
					"The Vulkan sampler or Vulkan image reference for \"{}\" is empty for shader \"{}\"", 
					textureSampler.Name, 
					_currentShader->GetName()
				));

			const ImageVulkan* vulkanImage = static_cast<ImageVulkan*>(image.Get());
			const ImageSamplerVulkan* vulkanSampler = static_cast<ImageSamplerVulkan*>(sampler.Get());

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

	bool RenderContextVulkan::HandlePurgeResources()
	{
		_renderCache->PurgeResources();

		return false;
	}
}