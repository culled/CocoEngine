#include "Renderpch.h"
#include "VulkanCommandBufferPool.h"
#include "VulkanGraphicsDevice.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanGraphicsDevice& device, VkQueue queue, uint32 queueFamilyIndex) :
		_device(device),
		_queue(queue),
		_queueFamilyIndex(queueFamilyIndex),
		_pool(nullptr),
		_allocatedBuffers{}
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = _queueFamilyIndex;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		AssertVkSuccess(vkCreateCommandPool(_device.GetDevice(), &createInfo, _device.GetAllocationCallbacks(), &_pool));
	}

	VulkanCommandBufferPool::~VulkanCommandBufferPool()
	{
		_device.WaitForIdle();

		if (_allocatedBuffers.size() > 0)
		{
			vkFreeCommandBuffers(_device.GetDevice(), _pool, static_cast<uint32_t>(_allocatedBuffers.size()), _allocatedBuffers.data());
		}

		_allocatedBuffers.clear();

		if (_pool)
		{
			vkDestroyCommandPool(_device.GetDevice(), _pool, _device.GetAllocationCallbacks());
			_pool = nullptr;
		}
	}

	UniqueRef<VulkanCommandBuffer> VulkanCommandBufferPool::Allocate(bool topLevel)
	{
		VkCommandBuffer buffer;
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.commandPool = _pool;
		allocateInfo.level = topLevel ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocateInfo.commandBufferCount = 1;

		AssertVkSuccess(vkAllocateCommandBuffers(_device.GetDevice(), &allocateInfo, &buffer));

		_allocatedBuffers.push_back(buffer);
		return CreateUniqueRef<VulkanCommandBuffer>(buffer, _queue);
	}

	void VulkanCommandBufferPool::Free(const VulkanCommandBuffer& buffer)
	{
		VkCommandBuffer cmdBuffer = buffer.GetCmdBuffer();
		const auto it = std::find(_allocatedBuffers.cbegin(), _allocatedBuffers.cend(), cmdBuffer);

		if (it == _allocatedBuffers.cend())
		{
			CocoError("CommandBuffer was not allocated from this pool")
			return;
		}

		_allocatedBuffers.erase(it);
		vkFreeCommandBuffers(_device.GetDevice(), _pool, 1, &cmdBuffer);
	}
}