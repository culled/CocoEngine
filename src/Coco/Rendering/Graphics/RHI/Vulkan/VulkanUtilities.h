#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Version.h>
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>
#include <Coco/Rendering/Graphics/ImageTypes.h>
#include <Coco/Rendering/ShaderTypes.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineTypes.h>
#include <Coco/Rendering/Graphics/BufferTypes.h>
#include <Coco/Rendering/Graphics/ImageSamplerTypes.h>
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
	/// Converts a ShaderStageType to a VkShaderStageFlagBits
	/// </summary>
	/// <param name="stage">The stage</param>
	/// <returns>The converted VkShaderStageFlagBits</returns>
	VkShaderStageFlagBits ToVkShaderStageFlagBits(ShaderStageType stage);

	/// <summary>
	/// Converts a PolygonFillMode to a VkPolygonMode
	/// </summary>
	/// <param name="fillMode">The fill mode</param>
	/// <returns>The converted VkPolygonMode</returns>
	VkPolygonMode ToVkPolygonMode(PolygonFillMode fillMode);

	/// <summary>
	/// Converts a TopologyMode to a VkPrimitiveTopology
	/// </summary>
	/// <param name="topologyMode">The topology mode</param>
	/// <returns>The converted VkPrimitiveTopology</returns>
	VkPrimitiveTopology ToVkPrimativeTopology(TopologyMode topologyMode);

	/// <summary>
	/// Converts a CullMode to a VkCullModeFlags
	/// </summary>
	/// <param name="cullMode">The cull mode</param>
	/// <returns>The converted VkCullModeFlags</returns>
	VkCullModeFlags ToVkCullModeFlags(CullMode cullMode);

	/// <summary>
	/// Converts a DepthTestingMode to a VkCompareOp
	/// </summary>
	/// <param name="depthMode">The depth mode</param>
	/// <returns>The converted VkCompareOp</returns>
	VkCompareOp ToVkCompareOp(DepthTestingMode depthMode);

	/// <summary>
	/// Converts a BlendOperation to a VkBlendOp
	/// </summary>
	/// <param name="blendOperation">The blend operation</param>
	/// <returns>The converted VkBlendOp</returns>
	VkBlendOp ToVkBlendOp(BlendOperation blendOperation);

	/// <summary>
	/// Converts a BlendFactorMode to a VkBlendFactor
	/// </summary>
	/// <param name="blendFactorMode">The blend factor mode</param>
	/// <returns>The converted VkBlendFactor</returns>
	VkBlendFactor ToVkBlendFactor(BlendFactorMode blendFactorMode);

	/// <summary>
	/// Converts a BufferDataFormat to a VkFormat
	/// </summary>
	/// <param name="dataFormat">The buffer data format</param>
	/// <returns>The converted VkFormat</returns>
	VkFormat ToVkFormat(BufferDataFormat dataFormat);

	/// <summary>
	/// Converts BufferUsageFlags to VkBufferUsageFlags
	/// </summary>
	/// <param name="usageFlags">The buffer usage flags</param>
	/// <returns>The converted VkBufferUsageFlags</returns>
	VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usageFlags);

	/// <summary>
	/// Converts ImageUsageFlags to VkImageUsageFlags
	/// </summary>
	/// <param name="usageFlags">The image usage flags</param>
	/// <returns>The converted VkImageUsageFlags</returns>
	VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags);

	VkFilter ToVkFilter(FilterMode filterMode);

	VkSamplerMipmapMode ToVkSamplerMipmapMode(FilterMode filterMode);

	VkSamplerAddressMode ToVkSamplerAddressMode(RepeatMode repeatMode);

	VkImageType ToVkImageType(ImageDimensionType dimensionType);

	VkDescriptorType ToVkDescriptorType(ShaderDescriptorType descriptorType);
}