#include "VulkanQueue.h"

namespace Coco::Rendering
{
	VulkanQueue::VulkanQueue(VkQueue queue, int queueFamily) :
		Queue(queue), QueueFamily(queueFamily)
	{}
}