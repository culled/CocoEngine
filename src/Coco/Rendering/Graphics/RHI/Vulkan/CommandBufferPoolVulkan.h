#pragma once

#include <Coco/Rendering/Graphics/CommandBufferPool.h>

#include <Coco/Core/Types/List.h>
#include "Resources/CommandBufferVulkan.h"
#include "VulkanIncludes.h"
#include "VulkanQueue.h"

namespace Coco::Rendering::Vulkan
{
	class GraphicsDeviceVulkan;

	/// @brief Vulkan-implementation of a CommandBufferPool
	class CommandBufferPoolVulkan final : public CommandBufferPool
	{
	private:
		GraphicsDeviceVulkan* _device = nullptr;
		VkCommandPool _pool = nullptr;
		Ref<VulkanQueue> _queue;

		List<Ref<CommandBufferVulkan>> _allocatedBuffers;

	public:
		CommandBufferPoolVulkan(GraphicsDeviceVulkan* device, Ref<VulkanQueue> queue);
		~CommandBufferPoolVulkan() final;

		Ref<CommandBuffer> Allocate(bool isPrimary, const string& name = "CommandBuffer") final;
		void Free(const Ref<CommandBuffer>& buffer) noexcept final;
		void WaitForQueue() noexcept final;

		/// @brief Gets the underlying VkCommandPool
		/// @return The underlying pool
		VkCommandPool GetPool() const noexcept { return _pool; }

		/// @brief Gets the queue that this pool is for
		/// @return The queue that this pool serves
		VkQueue GetQueue() const noexcept { return _queue->Queue; }
	};
}