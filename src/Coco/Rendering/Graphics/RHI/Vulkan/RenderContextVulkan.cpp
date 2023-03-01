#include "RenderContextVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include "ImageVulkan.h"

namespace Coco::Rendering
{
	RenderContextVulkan::RenderContextVulkan(
		Ref<Rendering::RenderView> renderView, 
		GraphicsDeviceVulkan* device, 
		const Ref<RenderPipeline>& pipeline, 
		List<ImageVulkan*> attachments,
		CommandBufferVulkan* commandBuffer) : 
		RenderContext(renderView),
		_device(device),
		_commandBuffer(commandBuffer)
	{
		if (!_device->GetGraphicsCommandPool().has_value())
			throw Exception("A graphics command pool needs to be created for rendering");

		List<VkImageView> views;

		for (ImageVulkan* attachment : attachments)
		{
			views.Add(attachment->GetNativeView());
		}

		if (views.Count() == 0)
			throw Exception("No Vulkan image attachments were given");

		_renderPass = _device->GetRenderCache()->GetOrCreateRenderPass(pipeline);
	}

	void RenderContextVulkan::Begin()
	{
		if(_framebuffer == nullptr)
			throw Exception("No framebuffer was set");

		_commandBuffer->Begin(true, false);
		_commandBuffer->BeginRenderPass(_renderPass, _framebuffer, RenderView);
	}

	void RenderContextVulkan::End()
	{
		List<GraphicsSemaphore*> waitSemaphores;

		for (GraphicsSemaphoreVulkan* semaphore : _waitSemaphores)
			waitSemaphores.Add(semaphore);

		List<GraphicsSemaphore*> signalSemaphores;

		for (GraphicsSemaphoreVulkan* semaphore : _signalSemaphores)
			signalSemaphores.Add(semaphore);

		_commandBuffer->EndRenderPass();
		_commandBuffer->EndAndSubmit(waitSemaphores, signalSemaphores, _signalFence);
	}

	void RenderContextVulkan::SetFramebuffer(VkFramebuffer framebuffer)
	{
		_framebuffer = framebuffer;
	}

	void RenderContextVulkan::AddWaitSemaphore(GraphicsSemaphoreVulkan* semaphore)
	{
		_waitSemaphores.Add(semaphore);
	}

	void RenderContextVulkan::AddSignalSemaphore(GraphicsSemaphoreVulkan* semaphore)
	{
		_signalSemaphores.Add(semaphore);
	}

	void RenderContextVulkan::SetSignalFence(GraphicsFenceVulkan* fence)
	{
		_signalFence = fence;
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_renderPass = nullptr;
	}
}