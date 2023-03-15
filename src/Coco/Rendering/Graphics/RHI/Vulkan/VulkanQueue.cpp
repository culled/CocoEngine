#include "VulkanQueue.h"

namespace Coco::Rendering
{
	VulkanQueue::VulkanQueue(VkQueue queue, int queueFamily) noexcept :
		Queue(queue), QueueFamily(queueFamily)
	{}
}