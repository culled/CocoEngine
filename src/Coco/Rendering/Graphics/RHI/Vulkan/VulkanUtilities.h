#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Version.h>
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// Converts a VkPhysicalDeviceType to a GraphicsDeviceType
	/// </summary>
	/// <param name="deviceType">The device type</param>
	/// <returns>The converted device type</returns>
	GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType deviceType);

	/// <summary>
	/// Converts a version provided by Vulkan to a Version struct
	/// </summary>
	/// <param name="version">The Vulkan version</param>
	/// <returns>The converted version</returns>
	Version ToVersion(uint32_t version);

	/// <summary>
	/// Converts a Version struct to a Vulkan version
	/// </summary>
	/// <param name="version">The version</param>
	/// <returns>The converted Vulkan version</returns>
	uint32_t ToVkVersion(const Version& version);

	/// <summary>
	/// Converts a VerticalSyncMode to a VkPresentModeKHR
	/// </summary>
	/// <param name="vsyncMode">The vsync mode</param>
	/// <returns>The converted present mode</returns>
	VkPresentModeKHR ToVkPresentMode(VerticalSyncMode vsyncMode);

	/// <summary>
	/// Converts a VkPresentModeKHR to a VerticalSyncMode
	/// </summary>
	/// <param name="presentMode">The present mode</param>
	/// <returns>The converted vertical sync mode</returns>
	VerticalSyncMode ToVerticalSyncMode(VkPresentModeKHR presentMode);
}