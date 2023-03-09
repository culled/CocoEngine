#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Set.h>
#include <Coco/Rendering/Graphics/RenderContext.h>
#include "GraphicsFenceVulkan.h"
#include "GraphicsSemaphoreVulkan.h"
#include "CommandBufferVulkan.h"
#include "VulkanRenderCache.h"

#include <Coco/Rendering/Graphics/RenderContextTypes.h>
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
		VulkanRenderPass _renderPass;
		CommandBufferVulkan* _commandBuffer;
		VkFramebuffer _framebuffer = nullptr;
		VulkanPipeline _currentPipeline = VulkanPipeline::None;

		List<GraphicsSemaphoreVulkan*> _waitSemaphores;
		List<GraphicsSemaphoreVulkan*> _signalSemaphores;
		GraphicsFenceVulkan* _signalFence = nullptr;

		Set<RenderContextStateChange> _stateChanges;

		Ref<Shader> _currentShader;

	public:
		RenderContextVulkan(
			Ref<Rendering::RenderView> renderView, 
			GraphicsDeviceVulkan* device,
			const Ref<RenderPipeline>& pipeline,
			CommandBufferVulkan* commandBuffer);

		virtual ~RenderContextVulkan() override;

		virtual bool Begin() override;
		virtual void End() override;
		virtual void SetViewport(const Vector2Int& offset, const SizeInt& size) override;
		virtual void UseShader(Ref<Shader> shader) override;
		virtual void DrawIndexed(uint indexCount, uint indexOffset, uint vertexOffset, uint instanceCount, uint instanceOffset) override;
		virtual void Draw(const Ref<Mesh>& mesh) override;

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
		VulkanRenderPass GetRenderPass() const { return _renderPass; }

	private:
		/// <summary>
		/// Flushes all state changes and binds the current state
		/// </summary>
		void FlushStateChanges();
	};
}