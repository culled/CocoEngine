#include "RenderContextVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include "ImageVulkan.h"
#include <Coco/Core/Logging/Logger.h>
#include "GraphicsPlatformVulkan.h"
#include "BufferVulkan.h"
#include "VulkanShader.h"
#include <Coco/Rendering/Graphics/GraphicsResource.h>
#include <Coco/Core/Types/Array.h>

namespace Coco::Rendering
{
	RenderContextVulkan::RenderContextVulkan(GraphicsDevice* device) :
		_device(static_cast<GraphicsDeviceVulkan*>(device)), _currentState(RenderContextState::Ready)
	{
		if (!_device->GetGraphicsCommandPool(&_pool))
			throw Exception("A graphics command pool needs to be created for rendering");

		_commandBuffer = static_cast<CommandBufferVulkan*>(_pool->Allocate(true));

		_globalUBO = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform,
			sizeof(GlobalUniformObject),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			true);

		_objectUBO = _device->CreateResource<BufferVulkan>(
			BufferUsageFlags::TransferDestination | BufferUsageFlags::Uniform,
			sizeof(ShaderUniformObject) * 10,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			true);

		_imageAvailableSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteSemaphore = _device->CreateResource<GraphicsSemaphoreVulkan>();
		_renderingCompleteFence = _device->CreateResource<GraphicsFenceVulkan>(true);

		CreateGlobalDescriptorSet();
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_device->WaitForIdle();

		for (auto& poolKVP : _descriptorPools)
		{
			poolKVP.second.reset();
		}

		_descriptorSets.clear();
		_descriptorPools.clear();

		_globalDescriptorPool.reset();

		vkDestroyDescriptorSetLayout(_device->GetDevice(), _globalDescriptorSetLayout, nullptr);
		_globalDescriptorSetLayout = nullptr;

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

	void RenderContextVulkan::SetRenderTargets(const List<GraphicsResourceRef<ImageVulkan>>& renderTargets)
	{
		DestroyFramebuffer();
		_renderTargets = renderTargets;
	}

	void RenderContextVulkan::DrawIndexed(uint indexCount, uint indexOffset, uint vertexOffset, uint instanceCount, uint instanceOffset)
	{
		FlushStateChanges();

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset);
	}

	void RenderContextVulkan::Draw(const Ref<Mesh>& mesh, const Matrix4x4& modelMatrix)
	{
		FlushStateChanges();

		// Bind the vertex buffer
		VkDeviceSize offsets[1] = { 0 };

		GraphicsResourceRef<Buffer> vertexBufferRef = mesh->GetVertexBuffer();
		BufferVulkan* vertexBuffer = static_cast<BufferVulkan*>(vertexBufferRef.get());
		VkBuffer vertexVkBuffer = vertexBuffer->GetBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets);

		// Bind the index buffer
		GraphicsResourceRef<Buffer> indexBufferRef = mesh->GetIndexBuffer();
		BufferVulkan* indexBuffer = static_cast<BufferVulkan*>(indexBufferRef.get());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		// Push the model matrix
		Array<float, Matrix4x4::CellCount> modelMat = modelMatrix.AsFloat();
		vkCmdPushConstants(_commandBuffer->GetCmdBuffer(), _currentPipeline.Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * Matrix4x4::CellCount, modelMat.data());

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), static_cast<uint>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}

	bool RenderContextVulkan::IsAvaliableForRendering()
	{
		return _renderingCompleteFence->IsSignalled();
	}

	void RenderContextVulkan::WaitForRenderingCompleted()
	{
		_renderingCompleteFence->Wait(std::numeric_limits<uint64_t>::max());
	}

	bool RenderContextVulkan::BeginImpl()
	{
		_renderPass = _device->GetRenderCache()->GetOrCreateRenderPass(CurrentPipeline);

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

		VkClearValue clearValue = { {{
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

	void RenderContextVulkan::EndImpl()
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

	void RenderContextVulkan::ResetImpl()
	{
		_stateChanges.clear();
		_currentShader.reset();
		_signalSemaphores.Clear();
		_waitSemaphores.Clear();

		_waitSemaphores.Add(_imageAvailableSemaphore);
		_signalSemaphores.Add(_renderingCompleteSemaphore);
		_renderingCompleteFence->Reset();

		_currentState = RenderContextState::Ready;
	}

	void RenderContextVulkan::FlushStateChanges()
	{
		if (CurrentRenderPass == nullptr)
			throw Exception("A render pass was not set before rendering geometry");

		if (_stateChanges.contains(RenderContextStateChange::Shader))
		{
			_currentPipeline = _device->GetRenderCache()->GetOrCreatePipeline(
				_renderPass, 
				CurrentRenderPass->GetName(), 
				CurrentRenderPassIndex, 
				_currentShader, 
				_globalDescriptorSetLayout);

			vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _currentPipeline.Pipeline);

			// TODO: this needs to change
			if (!_descriptorSets.contains(_currentShader.get()))
				UpdateObjectDescriptorSet();

			Array<VkDescriptorSet, 2> sets = { _globalDescriptorSet, _descriptorSets[_currentShader.get()] };

			// Bind the descriptor sets
			vkCmdBindDescriptorSets(
				_commandBuffer->GetCmdBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_currentPipeline.Layout,
				0,
				static_cast<uint32_t>(sets.size()),
				sets.data(),
				0,
				0);
		}

		_stateChanges.clear();
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

		try
		{
			List<VkImageView> renderTargets;
			for (const GraphicsResourceRef<ImageVulkan> renderTarget : _renderTargets)
			{
				renderTargets.Add(renderTarget->GetNativeView());
			}

			framebufferInfo.attachmentCount = static_cast<uint32_t>(renderTargets.Count());
			framebufferInfo.pAttachments = renderTargets.Data();

			AssertVkResult(vkCreateFramebuffer(_device->GetDevice(), &framebufferInfo, nullptr, &_framebuffer));
		}
		catch (Exception& ex)
		{
			string err = FormattedString("Failed to create framebuffers: {}", ex.what());
			throw Exception(err.c_str());
		}
	}

	void RenderContextVulkan::DestroyFramebuffer()
	{
		_device->WaitForIdle();

		vkDestroyFramebuffer(_device->GetDevice(), _framebuffer, nullptr);
		_framebuffer = nullptr;
	}

	void RenderContextVulkan::UpdateObjectDescriptorSet()
	{
		if (!_descriptorPools.contains(_currentShader.get()))
		{
			GraphicsResourceRef<VulkanShader> vulkanShader = _device->GetRenderCache()->GetOrCreateVulkanShader(_currentShader);
			List<VkDescriptorSetLayout> descriptorSetLayouts = vulkanShader->GetDescriptorSetLayouts();

			_descriptorPools[_currentShader.get()] = _device->CreateResource<DescriptorPoolVulkan>(static_cast<uint>(descriptorSetLayouts.Count()), descriptorSetLayouts);
			_descriptorSets[_currentShader.get()] = _descriptorPools[_currentShader.get()]->GetOrAllocateSet(_rcHash(this));
		}

		// Update the descriptor sets
		VkDescriptorBufferInfo objectUBOBufferInfo = {};
		objectUBOBufferInfo.buffer = _objectUBO->GetBuffer();
		objectUBOBufferInfo.offset = 0;
		objectUBOBufferInfo.range = sizeof(ShaderUniformObject); // TODO

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = _descriptorSets[_currentShader.get()];
		descriptorWrite.dstBinding = 1;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &objectUBOBufferInfo;

		vkUpdateDescriptorSets(_device->GetDevice(), 1, &descriptorWrite, 0, nullptr);
	}

	void RenderContextVulkan::CreateGlobalDescriptorSet()
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = 1;
		layoutCreateInfo.pBindings = &binding;

		AssertVkResult(vkCreateDescriptorSetLayout(_device->GetDevice(), &layoutCreateInfo, nullptr, &_globalDescriptorSetLayout));

		List<VkDescriptorSetLayout> descriptorSetLayouts{ _globalDescriptorSetLayout };

		_globalDescriptorPool = _device->CreateResource<DescriptorPoolVulkan>(static_cast<uint>(descriptorSetLayouts.Count()), descriptorSetLayouts);
		_globalDescriptorSet = _globalDescriptorPool->GetOrAllocateSet(_rcHash(this));

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
}