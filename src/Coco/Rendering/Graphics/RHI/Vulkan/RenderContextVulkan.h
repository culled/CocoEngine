#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/RenderContext.h>
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "CommandBufferVulkan.h"

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	class RenderPipeline;
	class GraphicsDeviceVulkan;
	class ImageVulkan;

	/// <summary>
	/// Vulkan-implementation of a RenderContext
	/// </summary>
	class RenderContextVulkan : public RenderContext
	{
	private:
		GraphicsDeviceVulkan* _device;
		VkFramebuffer _framebuffer = nullptr;
		VkRenderPass _renderPass = nullptr;
		CommandBufferVulkan* _commandBuffer;

		List<GraphicsSemaphoreVulkan*> _waitSemaphores;
		List<GraphicsSemaphoreVulkan*> _signalSemaphores;
		GraphicsFenceVulkan* _signalFence = nullptr;

	public:
		RenderContextVulkan(
			Ref<Rendering::RenderView> renderView, 
			GraphicsDeviceVulkan* device,
			const Ref<RenderPipeline>& pipeline,
			List<ImageVulkan*> attachments,
			CommandBufferVulkan* commandBuffer);
		virtual ~RenderContextVulkan() override;

		virtual void Begin() override;
		virtual void End() override;
		virtual void SetViewport(const Vector2Int& offset, const SizeInt& size) override { _commandBuffer->SetViewport(offset, size); }

		void SetFramebuffer(VkFramebuffer framebuffer);
		void AddWaitSemaphore(GraphicsSemaphoreVulkan* semaphore);
		void AddSignalSemaphore(GraphicsSemaphoreVulkan* semaphore);
		void SetSignalFence(GraphicsFenceVulkan* fence);

		VkRenderPass GetRenderPass() const { return _renderPass; }
	};
}