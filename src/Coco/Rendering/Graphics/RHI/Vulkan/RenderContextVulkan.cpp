#include "RenderContextVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include <Coco/Rendering/Pipeline/IRenderPass.h>
#include "ImageVulkan.h"
#include <Coco/Core/Logging/Logger.h>
#include "GraphicsPlatformVulkan.h"
#include "BufferVulkan.h"

namespace Coco::Rendering
{
	RenderContextVulkan::RenderContextVulkan(
		Ref<Rendering::RenderView> renderView, 
		GraphicsDeviceVulkan* device, 
		const Ref<RenderPipeline>& pipeline,
		CommandBufferVulkan* commandBuffer) : 
		RenderContext(renderView),
		_device(device),
		_commandBuffer(commandBuffer)
	{
		CommandBufferPoolVulkan* pool;
		if (!_device->GetGraphicsCommandPool(&pool))
			throw Exception("A graphics command pool needs to be created for rendering");

		_renderPass = _device->GetRenderCache()->GetOrCreateRenderPass(pipeline);
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_signalSemaphores.Clear();
		_waitSemaphores.Clear();
	}

	bool RenderContextVulkan::Begin()
	{
		if (_framebuffer == nullptr)
		{
			LogError(_device->VulkanPlatform->GetLogger(), "No framebuffer was set");
			return false;
		}

		_commandBuffer->Begin(true, false);
		_commandBuffer->CmdBeginRenderPass(_renderPass.RenderPass, _framebuffer, RenderView);

		return true;
	}

	void RenderContextVulkan::End()
	{
		List<GraphicsSemaphore*> waitSemaphores;

		for (GraphicsSemaphoreVulkan* semaphore : _waitSemaphores)
			waitSemaphores.Add(semaphore);

		List<GraphicsSemaphore*> signalSemaphores;

		for (GraphicsSemaphoreVulkan* semaphore : _signalSemaphores)
			signalSemaphores.Add(semaphore);

		_commandBuffer->CmdEndRenderPass();
		_commandBuffer->EndAndSubmit(waitSemaphores, signalSemaphores, _signalFence);
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

	void RenderContextVulkan::DrawIndexed(uint indexCount, uint indexOffset, uint vertexOffset, uint instanceCount, uint instanceOffset)
	{
		FlushStateChanges();

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), indexCount, instanceCount, indexOffset, vertexOffset, instanceOffset);
	}

	void RenderContextVulkan::Draw(const Ref<Mesh>& mesh)
	{
		FlushStateChanges();

		// Bind the vertex buffer
		VkDeviceSize offsets[1] = { 0 };
		BufferVulkan* vertexBuffer = static_cast<BufferVulkan*>(mesh->GetVertexBuffer());
		VkBuffer vertexVkBuffer = vertexBuffer->GetCmdBuffer();
		vkCmdBindVertexBuffers(_commandBuffer->GetCmdBuffer(), 0, 1, &vertexVkBuffer, offsets);

		// Bind the index buffer
		BufferVulkan* indexBuffer = static_cast<BufferVulkan*>(mesh->GetIndexBuffer());
		vkCmdBindIndexBuffer(_commandBuffer->GetCmdBuffer(), indexBuffer->GetCmdBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(_commandBuffer->GetCmdBuffer(), static_cast<uint>(mesh->GetIndexCount()), 1, 0, 0, 0);
	}

	void RenderContextVulkan::AddWaitSemaphore(GraphicsSemaphoreVulkan* semaphore)
	{
		_waitSemaphores.Add(semaphore);
	}

	void RenderContextVulkan::AddSignalSemaphore(GraphicsSemaphoreVulkan* semaphore)
	{
		_signalSemaphores.Add(semaphore);
	}

	void RenderContextVulkan::FlushStateChanges()
	{
		if (CurrentRenderPass == nullptr)
			throw Exception("A render pass was not set before rendering geometry");

		if (_stateChanges.contains(RenderContextStateChange::Shader))
		{
			_currentPipeline = _device->GetRenderCache()->GetOrCreatePipeline(_renderPass, CurrentRenderPass->GetName(), CurrentRenderPassIndex, _currentShader);
			vkCmdBindPipeline(_commandBuffer->GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, _currentPipeline.Pipeline);
		}

		_stateChanges.clear();
	}
}