#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Rendering/Graphics/CommandBuffer.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	struct RenderView;
	class CommandBufferPoolVulkan;
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a CommandBuffer
	/// </summary>
	class CommandBufferVulkan : public CommandBuffer
	{
	private:
		VkCommandBuffer _commandBuffer = nullptr;
		GraphicsDeviceVulkan* _device;
		CommandBufferPoolVulkan* _pool;

		bool _isInRenderPass = false;

	public:
		CommandBufferVulkan(bool isPrimary, GraphicsDeviceVulkan* device, CommandBufferPoolVulkan* pool);
		virtual ~CommandBufferVulkan() override;

		virtual void Begin(bool isSingleUse, bool isSimultaneousUse) override;
		virtual void End() override;
		virtual void Submit(const List<GraphicsSemaphore*>& waitSemaphores = {}, const List<GraphicsSemaphore*>& signalSemaphores = {}, GraphicsFence* signalFence = nullptr) override;
		virtual void Reset() override;

		/// <summary>
		/// Gets the underlying Vulkan command buffer
		/// </summary>
		/// <returns>The underlying Vulkan command buffer</returns>
		VkCommandBuffer GetCmdBuffer() const { return _commandBuffer; }

		/// <summary>
		/// Begins a render pass
		/// </summary>
		/// <param name="renderPass">The render pass</param>
		/// <param name="framebuffer">The framebuffer to use</param>
		/// <param name="renderView">The render view</param>
		void CmdBeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer, const Ref<RenderView>& renderView);

		/// <summary>
		/// Ends the current render pass
		/// </summary>
		void CmdEndRenderPass();
	};
}