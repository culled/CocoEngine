#include "VulkanQueue.h"

namespace Coco::Rendering::Vulkan
{
	VulkanQueue::VulkanQueue(VkQueue queue, int queueFamily) noexcept :
		Queue(queue), QueueFamily(queueFamily)
	{}
}