#pragma once

#include <Coco/Core/Core.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief A wrapper for a Vulkan queue
	struct VulkanQueue
	{
		/// @brief The Vulkan queue
		const VkQueue Queue;

		/// @brief The queue family that this queue belongs to
		const int QueueFamily;

		VulkanQueue(VkQueue queue, int queueFamily) noexcept;
	};
}

