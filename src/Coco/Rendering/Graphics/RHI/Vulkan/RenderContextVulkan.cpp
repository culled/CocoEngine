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

// TODO: how to calculate this?
#define MAX_OBJECT_COUNT 1024
#define MAX_MATERIAL_SIZE 256

namespace Coco::Rendering
{
	RenderContextVulkan::RenderContextVulkan(GraphicsDevice* device) :
		_device(static_cast<GraphicsDeviceVulkan*>(device)), _currentState(RenderContextState::Ready)
	{
		_renderingService = RenderingService::Get();
		if (_renderingService == nullptr)
			throw RenderContextCreateException("Could not find an active rendering service");

		if (!_device->GetGraphicsCommandPool(_pool))
			throw RenderContextCreateException("A graphics command pool needs to be created for rendering");

		_commandBuffer = static_cast<CommandBufferVulkan*>(_pool->Allocate(true));

		_globalUBO = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			sizeof(GlobalUniformObject),
			true);

		_objectUBO = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform | BufferUsageFlags::HostVisible,
			MAX_MATERIAL_SIZE * MAX_OBJECT_COUNT,
			true);

		_imageAvailableSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteFence = _device->CreateResource<GraphicsFenceVulkan>(true);

		CreateGlobalDescriptorSet();
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_device->WaitForIdle();

		_globalDescriptorPool.reset();

		vkDestroyDescriptorSetLayout(_device->GetDevice(), _globalDescriptor.Layout, nullptr);
		_globalDescriptor.Layout = nullptr;

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
		_objectUBO.reset();

		DestroyFramebuffer();
	}

	void RenderContextVulkan::SetViewport(const Vector2Int& offset, const SizeInt& size)
	{
		VkViewport viewport = {};
		viewport.x = static_cast<float>(offset.X);
		viewport.y = static_cast<float>(offset.Y);
		viewport.width = static_cast<float>(size.Width);
		viewport.height = static_cast<float>(size.Height);
		viewport.minDepth = 0.0f; // TODO
		viewport.maxDepth = 1.0f; // TODO

		vkCmdSetViewport(_commandBuffer->GetCmdBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = static_cast<uint32_t>(size.Width);
		scissor.extent.height = static_cast<uint32_t>(size.Height);

		vkCmdSetScissor(_commandBuffer->GetCmdBuffer(), 0, 1, &scissor);
	}

	void RenderContextVulkan::UseShader(Ref<Shader> shader)
	{
		_stateChanges.insert(RenderContextStateChange::Shader);
		_currentShader = shader;
	}

	void RenderContextVulkan::UseMaterial(Ref<Material> material)
	{
		_stateChanges.insert(RenderContextStateChange::Material);
		_currentMaterial = material;
		
		if (material->GetShader() != _currentShader)
			UseShader(material->GetShader());
	}

	void RenderContextVulkan::SetRenderTargets(const List<GraphicsResourceRef<ImageVulkan>>& renderTargets)
	{
		DestroyFramebuffer();
		_renderTargets = renderTargets;
	}

	void RenderContextVulkan::Draw(const Mesh* mesh, const Matrix4x4& modelMatrix)
	{
		if (!FlushStateChanges())
		{
			LogError(_renderingService->GetLogger(), "Failed setting up state for rendering mesh. Skipping...");
			return;
		}

		// Bind the vertex buffer
		Array<VkDeviceSize, 1> offsets = { 0 };

		GraphicsResourceRef<Buffer> vertexBufferRef = mesh->GetVertexBuffer();
		const BufferVulkan* vertexBuffer = static_cast<BufferVulkan*>(vertexBufferRef.get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets.data());

		// Bind the index buffer
		GraphicsResourceRef<Buffer> indexBufferRef = mesh->GetIndexBuffer();
		const BufferVulkan* indexBuffer = static_cast<BufferVulkan*>(indexBufferRef.get());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Push the model matrix
		Array<float, Matrix4x4::CellCount> modelMat = modelMatrix.AsFloat();
		vkCmdPushConstants(_commandBuffer->GetCmdBuffer(), _currentPipeline.Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * Matrix4x4::CellCount, modelMat.data());

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), static_cast<uint>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}

	void RenderContextVulkan::WaitForRenderingCompleted() noexcept
	{
		try
		{
			_renderingCompleteFence->Wait(std::numeric_limits<uint64_t>::max());
		}
		catch(...)
		{ }
	}

	bool RenderContextVulkan::BeginImpl() noexcept
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
		catch (const Exception& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to begin VulkanRenderContext: {}", ex.what()));
			return false;
		}
	}

	void RenderContextVulkan::EndImpl() noexcept
	{
		try
		{
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
		catch(const Exception& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to end VulkanRenderContext: {}", ex.what()));
		}
	}

	void RenderContextVulkan::ResetImpl() noexcept
	{
		// Free all material descriptor sets
		for (auto& poolKVP : _shaderDescriptorPools)
		{
			poolKVP.second->FreeSets();
		}

		_shaderDescriptorPools.clear();
		_materialDescriptorSets.clear();
		_currentObjectUBOOffset = 0;

		// Clear all currently bound items
		_stateChanges.clear();
		_currentShader.reset();
		_currentMaterial.reset();
		_currentPipeline = VulkanPipeline::None;

		// Reset render syncronization objects
		_signalSemaphores.Clear();
		_waitSemaphores.Clear();

		try
		{
			_waitSemaphores.Add(_imageAvailableSemaphore);
			_signalSemaphores.Add(_renderingCompleteSemaphore);
			_renderingCompleteFence->Reset();

			_currentState = RenderContextState::Ready;
		}
		catch(const Exception& ex)
		{
			LogError(_device->VulkanPlatform->GetLogger(), FormattedString("Unable to reset VulkanRenderContext: {}", ex.what()));
		}
	}

	bool RenderContextVulkan::FlushStateChanges() noexcept
	{
		try
		{
			if (CurrentRenderPass == nullptr)
				throw Exception("A render pass was not set before rendering geometry");

			if (_stateChanges.contains(RenderContextStateChange::Shader))
			{
				_currentPipeline = _device->GetRenderCache()->GetOrCreatePipeline(
					_renderPass,
					CurrentRenderPass->GetName(),
					CurrentRenderPassIndex,
					_currentShader.get(),
					_globalDescriptor.Layout);

				vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _currentPipeline.Pipeline);

				// Bind the descriptor sets
				vkCmdBindDescriptorSets(
					_commandBuffer->GetCmdBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					_currentPipeline.Layout,
					0, 1, &_globalDescriptorSet,
					0, 0);
			}

			if (_stateChanges.contains(RenderContextStateChange::Material))
			{
				VkDescriptorSet set = GetOrAllocateMaterialDescriptorSet();

				// Bind the descriptor sets
				vkCmdBindDescriptorSets(
					_commandBuffer->GetCmdBuffer(),
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					_currentPipeline.Layout,
					1, 1, &set,
					0, 0);
			}

			_stateChanges.clear();

			return true;
		}
		catch (const Exception& ex)
		{
			_stateChanges.clear();
			LogError(_renderingService->GetLogger(), ex.what());
			return false;
		}
		catch (...)
		{
			_stateChanges.clear();
			LogError(_renderingService->GetLogger(), "Failed binding pipeline state");
			return false;
		}
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

		_globalDescriptorPool = _device->CreateResource<DescriptorPoolVulkan>(1, List<VulkanDescriptorLayout>({ _globalDescriptor }));
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

	VkDescriptorSet RenderContextVulkan::GetOrAllocateMaterialDescriptorSet()
	{
		const string subshaderName = CurrentRenderPass->GetName();

		GraphicsResourceRef<DescriptorPoolVulkan> pool;
		const ResourceID shaderID = _currentShader->GetID();
		const ResourceID materialID = _currentMaterial->GetID();

		GraphicsResourceRef<VulkanShader> vulkanShader = _device->GetRenderCache()->GetOrCreateVulkanShader(_currentShader.get());
		pool = vulkanShader->GetDescriptorPool();

		if (!_shaderDescriptorPools.contains(shaderID))
		{
			_shaderDescriptorPools[shaderID] = pool;
		}
		else if (_materialDescriptorSets.contains(materialID))
		{
			return _materialDescriptorSets[materialID];
		}

		const Subshader* subshader;
		if (!_currentShader->TryGetSubshader(subshaderName, subshader))
			throw Exception(FormattedString("Could not find a subshader in \"{}\" for pass {}", 
				_currentShader->GetName(),
				subshaderName,
				_currentShader->GetName()
			));

		VulkanDescriptorLayout layout;
		if (!vulkanShader->TryGetDescriptorSetLayout(subshaderName, layout))
			throw Exception(FormattedString("Could not find a descriptor layout for subshader \"{}\" in shader \"{}\"",
				subshaderName,
				_currentShader->GetName()
			));

		VkDescriptorSet set = pool->GetOrAllocateSet(layout, materialID);

		List<VkWriteDescriptorSet> descriptorWrites(subshader->Descriptors.Count());
		List<VkDescriptorBufferInfo> bufferInfos(subshader->Descriptors.Count());
		List<VkDescriptorImageInfo> imageInfos(subshader->Descriptors.Count());
		
		for (uint32_t i = 0; i < subshader->Descriptors.Count(); i++)
		{
			const ShaderDescriptor& descriptor = subshader->Descriptors[i];

			VkWriteDescriptorSet& write = descriptorWrites[i];
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = set;
			write.dstBinding = i;
			write.descriptorType = ToVkDescriptorType(descriptor.Type);
			write.descriptorCount = 1;

			switch (descriptor.Type)
			{
			case ShaderDescriptorType::UniformStruct:
			{
				List<uint8_t> data = _currentMaterial->GetStructData(descriptor.Name);
				if (data.Count() == 0)
				{
					LogError(_renderingService->GetLogger(), FormattedString("No data was set for material descriptor \"{}\"", descriptor.Name));
					continue;
				}

				VkDescriptorBufferInfo& objectUBOBufferInfo = bufferInfos[i];
				objectUBOBufferInfo.buffer = _objectUBO->GetBuffer();
				objectUBOBufferInfo.offset = _currentObjectUBOOffset;
				objectUBOBufferInfo.range = static_cast<uint32_t>(data.Count());

				// TODO: need to not make blocking
				_objectUBO->LoadData(_currentObjectUBOOffset, data);

				_currentObjectUBOOffset += objectUBOBufferInfo.range;

				write.pBufferInfo = &objectUBOBufferInfo;

				break;
			}
			case ShaderDescriptorType::UniformSampler:
			{
				Ref<Texture> texture = _currentMaterial->GetTexture(descriptor.Name);

				if (!texture)
					texture = _renderingService->GetDefaultTexture();

				GraphicsResourceRef<Image> image = texture->GetImage();
				const ImageVulkan* vulkanImage = static_cast<ImageVulkan*>(image.get());
				GraphicsResourceRef<ImageSampler> sampler = texture->GetSampler();
				const ImageSamplerVulkan* vulkanSampler = static_cast<ImageSamplerVulkan*>(sampler.get());

				// Texture data
				VkDescriptorImageInfo& imageInfo = imageInfos[i];
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = vulkanImage->GetNativeView();
				imageInfo.sampler = vulkanSampler->GetSampler();

				write.pImageInfo = &imageInfo;

				break;
			}
			default:
				continue;
			}
		}

		vkUpdateDescriptorSets(_device->GetDevice(), static_cast<uint32_t>(descriptorWrites.Count()), descriptorWrites.Data(), 0, nullptr);

		return set;
	}
}