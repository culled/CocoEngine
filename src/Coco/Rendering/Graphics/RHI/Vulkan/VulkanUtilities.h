#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Version.h>
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>
#include <Coco/Rendering/Graphics/ImageTypes.h>
#include <Coco/Rendering/Graphics/ShaderTypes.h>
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

	/// <summary>
	/// Converts a VkFormat to a PixelFormat
	/// </summary>
	/// <param name="format">The format</param>
	/// <returns>The converted PixelFormat</returns>
	PixelFormat ToPixelFormat(VkFormat format);

	/// <summary>
	/// Converts a PixelFormat to a VkFormat
	/// </summary>
	/// <param name="format">The format</param>
	/// <returns>The converted VkFormat</returns>
	VkFormat ToVkFormat(PixelFormat format);

	/// <summary>
	/// Converts a VkColorSpaceKHR to a ColorSpace
	/// </summary>
	/// <param name="colorSpace">The color space</param>
	/// <returns>The converted ColorSpace</returns>
	ColorSpace ToColorSpace(VkColorSpaceKHR colorSpace);

	/// <summary>
	/// Converts a ColorSpace to a VkColorSpaceKHR
	/// </summary>
	/// <param name="colorSpace">The color space</param>
	/// <returns>The converted VkColorSpaceKHR</returns>
	VkColorSpaceKHR ToVkColorSpace(ColorSpace colorSpace);

	/// <summary>
	/// Converts a ShaderStageType to a VkShaderStageFlags
	/// </summary>
	/// <param name="stage">The stage</param>
	/// <returns>The converted VkShaderStageFlags</returns>
	VkShaderStageFlags ToVkShaderStageFlags(ShaderStageType stage);
}