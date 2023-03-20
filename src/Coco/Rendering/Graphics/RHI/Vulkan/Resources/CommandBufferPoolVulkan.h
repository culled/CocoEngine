#pragma once

#include <Coco/Rendering/Graphics/Resources/CommandBufferPool.h>

#include <Coco/Core/Types/List.h>
#include "CommandBufferVulkan.h"
#include "../VulkanQueue.h"
#include "../VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a CommandBufferPool
	/// </summary>
	class CommandBufferPoolVulkan final : public CommandBufferPool
	{
	private:
		GraphicsDeviceVulkan* _device;
		VkCommandPool _pool = nullptr;
		Ref<VulkanQueue> _queue;

		List<Managed<CommandBufferVulkan>> _allocatedBuffers;

	public:
		CommandBufferPoolVulkan(GraphicsDeviceVulkan* device, Ref<VulkanQueue> queue);
		~CommandBufferPoolVulkan() final;

		CommandBuffer* Allocate(bool isPrimary) final;
		void Free(CommandBuffer* buffer) noexcept final;
		void WaitForQueue() noexcept final;

		/// <summary>
		/// Gets the underlying VkCommandPool
		/// </summary>
		/// <returns>The underlying pool</returns>
		VkCommandPool GetPool() const noexcept { return _pool; }

		/// <summary>
		/// Gets the queue that this pool is for
		/// </summary>
		/// <returns>The queue that this pool serves</returns>
		VkQueue GetQueue() const noexcept { return _queue->Queue; }
	};
}