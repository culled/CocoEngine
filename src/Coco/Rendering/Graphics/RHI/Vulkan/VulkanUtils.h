#pragma once

#include "VulkanIncludes.h"
#include "../../GraphicsDeviceTypes.h"
#include "../../ImageTypes.h"
#include "../../GraphicsPresenterTypes.h"
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

	/// @brief Converts an ImagePixelFormat and ImageColorSpace to a VkFormat
	/// @param pixelFormat The pixel format
	/// @param colorSpace The color space
	/// @return The format
	VkFormat ToVkFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace);

	/// @brief Converts a VkFormat to an ImagePixelFormat
	/// @param format The format
	/// @return The pixel format
	ImagePixelFormat ToImagePixelFormat(VkFormat format);

	/// @brief Converts a VkColorSpaceKHR to an ImageColorSpace
	/// @param colorSpace The color space 
	/// @return The converted color space
	ImageColorSpace ToImageColorSpace(VkColorSpaceKHR colorSpace);

	/// @brief Converts a VSyncMode to a VkPresentModeKHR
	/// @param vsyncMode The v-sync mode
	/// @return The converted present mode
	VkPresentModeKHR ToVkPresentMode(VSyncMode vsyncMode);

	/// @brief Converts a VkPresentModeKHR to a VSyncMode
	/// @param presentMode The present mode
	/// @return The v-sync mode
	VSyncMode ToVSyncMode(VkPresentModeKHR presentMode);
}