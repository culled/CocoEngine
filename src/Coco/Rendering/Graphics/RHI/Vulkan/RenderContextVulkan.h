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
			CommandBufferVulkan* commandBuffer);
		virtual ~RenderContextVulkan() override;

		virtual bool Begin() override;
		virtual void End() override;
		virtual void SetViewport(const Vector2Int& offset, const SizeInt& size) override { _commandBuffer->CmdSetViewport(offset, size); }

		/// <summary>
		/// Sets the framebuffer for this render context to use
		/// </summary>
		/// <param name="framebuffer">The framebuffer to use</param>
		void SetFramebuffer(VkFramebuffer framebuffer) { _framebuffer = framebuffer; }

		/// <summary>
		/// Adds a semaphore to use for waiting before rendering begins
		/// </summary>
		/// <param name="semaphore">The semaphore to wait on</param>
		void AddWaitSemaphore(GraphicsSemaphoreVulkan* semaphore);

		/// <summary>
		/// Adds a semaphore to use for signalling when rendering has completed
		/// </summary>
		/// <param name="semaphore">The semaphore to signal</param>
		void AddSignalSemaphore(GraphicsSemaphoreVulkan* semaphore);

		/// <summary>
		/// Sets the fence to use for signalling when rendering has completed
		/// </summary>
		/// <param name="fence">The fence to signal</param>
		void SetSignalFence(GraphicsFenceVulkan* fence) { _signalFence = fence; }

		/// <summary>
		/// Gets the render pass being used for this context
		/// </summary>
		/// <returns>The render pass being used</returns>
		VkRenderPass GetRenderPass() const { return _renderPass; }
	};
}