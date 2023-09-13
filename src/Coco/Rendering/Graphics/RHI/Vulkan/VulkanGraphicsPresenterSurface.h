#pragma once

#include "../../GraphicsPresenterTypes.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief A Vulkan-specific GraphicsPresenterSurface
	struct VulkanGraphicsPresenterSurface : public GraphicsPresenterSurface
	{
		/// @brief The Vulkan surface
		VkSurfaceKHR Surface;

		VulkanGraphicsPresenterSurface();
	};
}