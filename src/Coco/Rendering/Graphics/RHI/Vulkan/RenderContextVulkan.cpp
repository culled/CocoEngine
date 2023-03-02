#include "RenderContextVulkan.h"

#include "GraphicsDeviceVulkan.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>
#include "ImageVulkan.h"
#include <Coco/Core/Logging/Logger.h>
#include "GraphicsPlatformVulkan.h"

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
		if (!_device->GetGraphicsCommandPool().has_value())
			throw Exception("A graphics command pool needs to be created for rendering");

		_renderPass = _device->GetRenderCache()->GetOrCreateRenderPass(pipeline);
	}

	bool RenderContextVulkan::Begin()
	{
		if (_framebuffer == nullptr)
		{
			LogError(_device->VulkanPlatform->GetLogger(), "No framebuffer was set");
			return false;
		}

		_commandBuffer->Begin(true, false);
		_commandBuffer->BeginRenderPass(_renderPass, _framebuffer, RenderView);

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

		_commandBuffer->EndRenderPass();
		_commandBuffer->EndAndSubmit(waitSemaphores, signalSemaphores, _signalFence);
	}

	void RenderContextVulkan::AddWaitSemaphore(GraphicsSemaphoreVulkan* semaphore)
	{
		_waitSemaphores.Add(semaphore);
	}

	void RenderContextVulkan::AddSignalSemaphore(GraphicsSemaphoreVulkan* semaphore)
	{
		_signalSemaphores.Add(semaphore);
	}

	RenderContextVulkan::~RenderContextVulkan()
	{
		_signalSemaphores.Clear();
		_waitSemaphores.Clear();
		_renderPass = nullptr;
	}
}