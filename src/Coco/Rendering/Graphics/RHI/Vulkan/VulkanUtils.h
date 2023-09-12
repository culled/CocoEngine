#pragma once

#include "VulkanIncludes.h"
#include <Coco/Rendering/Graphics/GraphicsDeviceTypes.h>
#include <Coco/Core/Types/Version.h>

namespace Coco::Rendering::Vulkan
{
	/// @brief Converts a VkPhysicalDeviceType to a GraphicsDeviceType
	/// @param deviceType The device type
	/// @return The converted device type
	GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType deviceType);

	/// @brief Converts a version provided by Vulkan to a Version struct
	/// @param version The Vulkan version
	/// @return The converted version
	Version ToVersion(uint32_t version);

	/// @brief Converts a Version struct to a Vulkan version
	/// @param version The version
	/// @return The converted Vulkan version
	uint32_t ToVkVersion(const Version& version);
}