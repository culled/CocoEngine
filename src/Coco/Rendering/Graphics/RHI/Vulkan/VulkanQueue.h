#pragma once

#include <Coco/Core/Core.h>

#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// A wrapper for a Vulkan queue
	/// </summary>
	struct VulkanQueue
	{
		/// <summary>
		/// The Vulkan queue
		/// </summary>
		VkQueue Queue;

		/// <summary>
		/// The queue family that this queue belongs to
		/// </summary>
		int QueueFamily;

		VulkanQueue(VkQueue queue, int queueFamily) noexcept;
	};
}

