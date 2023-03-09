#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Rendering/Graphics/CommandBufferPool.h>

#include "VulkanQueue.h"
#include "VulkanIncludes.h"
#include "CommandBufferVulkan.h"

namespace Coco::Rendering
{
	class GraphicsDeviceVulkan;

	/// <summary>
	/// Vulkan-implementation of a CommandBufferPool
	/// </summary>
	class CommandBufferPoolVulkan : public CommandBufferPool
	{
	private:
		GraphicsDeviceVulkan* _device;
		VkCommandPool _pool = nullptr;
		Ref<VulkanQueue> _queue;

		List<Managed<CommandBufferVulkan>> _allocatedBuffers;

	public:
		CommandBufferPoolVulkan(GraphicsDeviceVulkan* device, Ref<VulkanQueue> queue);
		virtual ~CommandBufferPoolVulkan() override;

		virtual CommandBuffer* Allocate(bool isPrimary) override;
		virtual void Free(CommandBuffer* buffer) override;
		virtual void WaitForQueue() override;

		/// <summary>
		/// Gets the underlying VkCommandPool
		/// </summary>
		/// <returns>The underlying pool</returns>
		VkCommandPool GetPool() const { return _pool; }

		/// <summary>
		/// Gets the queue that this pool is for
		/// </summary>
		/// <returns>The queue that this pool serves</returns>
		VkQueue GetQueue() const { return _queue->Queue; }
	};
}