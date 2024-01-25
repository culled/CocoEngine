#include "Renderpch.h"
#include "VulkanQueue.h"
#include "VulkanGraphicsDevice.h"

namespace Coco::Rendering::Vulkan
{
	VulkanQueue::VulkanQueue(VulkanGraphicsDevice& device, uint32 familyIndex, VkQueue queue, VulkanQueueType type) :
		_device(device),
		_familyIndex(familyIndex),
		_queue(queue),
		_type(type)
	{
		_commandBufferPool = CreateManagedRef<VulkanCommandBufferPool>(0, device, *this);
	}

	VulkanQueue::~VulkanQueue()
	{
		_commandBufferPool.Invalidate();
	}

	void VulkanQueue::WaitForIdle()
	{
		try
		{
			AssertVkSuccess(vkQueueWaitIdle(_queue))
		}
		catch (const VulkanOperationException&)
		{
			_device.WaitForIdle();
		}
	}
}