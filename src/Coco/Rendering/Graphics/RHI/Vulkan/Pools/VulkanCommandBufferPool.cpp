#include "Renderpch.h"
#include "VulkanCommandBufferPool.h"
#include "../VulkanGraphicsDevice.h"
#include "../VulkanQueue.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanCommandBufferPool::VulkanCommandBufferPool(const GraphicsResourceID& id, VulkanGraphicsDevice& device, const VulkanQueue& queue) :
		GraphicsResource(id),
		_device(device),
		_queue(queue),
		_pool(nullptr),
		_allocatedBuffers()
	{
		VkCommandPoolCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		createInfo.queueFamilyIndex = queue.GetFamilyIndex();
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		AssertVkSuccess(vkCreateCommandPool(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_pool))
	}

	VulkanCommandBufferPool::~VulkanCommandBufferPool()
	{
		if (_pool)
		{
			_device.WaitForIdle();
			vkDestroyCommandPool(_device.GetDevice(), _pool, _device.GetAllocationCallbacks());
			_pool = nullptr;
			_allocatedBuffers.clear();
		}
	}

	VulkanCommandBuffer VulkanCommandBufferPool::Allocate(bool topLevel)
	{
		VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.commandPool = _pool;
		allocateInfo.level = topLevel ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocateInfo.commandBufferCount = 1;

		VkCommandBuffer buffer;
		AssertVkSuccess(vkAllocateCommandBuffers(_device.GetDevice(), &allocateInfo, &buffer))

		_allocatedBuffers.push_back(buffer);
		return VulkanCommandBuffer(buffer, _queue.GetQueue());
	}

	void VulkanCommandBufferPool::Free(const VulkanCommandBuffer& buffer)
	{
		auto it = std::find_if(_allocatedBuffers.begin(), _allocatedBuffers.end(), 
			[buffer](const VkCommandBuffer& other)
			{
				return other == buffer.GetCmdBuffer();
			});

		if (it == _allocatedBuffers.end())
		{
			CocoError("CommandBuffer was not allocated from this pool")
			return;
		}

		vkFreeCommandBuffers(_device.GetDevice(), _pool, 1, &(*it));
		_allocatedBuffers.erase(it);
	}
}