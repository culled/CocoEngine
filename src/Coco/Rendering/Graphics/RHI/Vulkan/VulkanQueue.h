#pragma once

#include <Coco/Core/Core.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	struct VulkanQueue
	{
		VkQueue Queue;
		int QueueFamily;

		VulkanQueue(VkQueue queue, int queueFamily);
	};
}

