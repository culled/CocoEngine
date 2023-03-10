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
	class CommandBufferVulkan final : public CommandBuffer
	{
	private:
		VkCommandBuffer _commandBuffer = nullptr;
		GraphicsDeviceVulkan* _device;
		CommandBufferPoolVulkan* _pool;

	public:
		CommandBufferVulkan(bool isPrimary, GraphicsDeviceVulkan* device, CommandBufferPoolVulkan* pool);
		~CommandBufferVulkan() final;

		void Begin(bool isSingleUse, bool isSimultaneousUse) final;
		void End() override;
		void Submit(
			const List<IGraphicsSemaphore*>& waitSemaphores = {},
			const List<IGraphicsSemaphore*>& signalSemaphores = {},
			IGraphicsFence* signalFence = nullptr) final;
		void Reset() final;

		/// <summary>
		/// Gets the underlying Vulkan command buffer
		/// </summary>
		/// <returns>The underlying Vulkan command buffer</returns>
		VkCommandBuffer GetCmdBuffer() const noexcept { return _commandBuffer; }
	};
}