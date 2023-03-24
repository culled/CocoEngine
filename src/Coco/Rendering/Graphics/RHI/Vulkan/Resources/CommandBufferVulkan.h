#pragma once

#include <Coco/Rendering/Graphics/Resources/CommandBuffer.h>

#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class CommandBufferPoolVulkan;
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a CommandBuffer
	class CommandBufferVulkan final : public CommandBuffer
	{
	private:
		VkCommandBuffer _commandBuffer = nullptr;
		GraphicsDeviceVulkan* _device;
		CommandBufferPoolVulkan* _pool;

	public:
		CommandBufferVulkan(bool isPrimary, GraphicsDeviceVulkan* device, CommandBufferPoolVulkan* pool);
		~CommandBufferVulkan() final;

		/// @brief Gets the underlying Vulkan command buffer
		/// @return The underlying Vulkan command buffer
		VkCommandBuffer GetCmdBuffer() const noexcept { return _commandBuffer; }

	protected:
		void BeginImpl(bool isSingleUse, bool isSimultaneousUse) final;
		void EndImpl() override;
		void SubmitImpl(
			const List<IGraphicsSemaphore*>& waitSemaphores,
			const List<IGraphicsSemaphore*>& signalSemaphores,
			IGraphicsFence* signalFence) final;
		void ResetImpl() final;
	};
}