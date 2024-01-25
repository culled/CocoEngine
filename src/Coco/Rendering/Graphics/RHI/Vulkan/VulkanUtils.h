#pragma once
#include <Coco/Core/Types/Version.h>
#include <Coco/Core/Types/Vector.h>
#include "../../GraphicsDeviceTypes.h"
#include "../../GraphicsPipelineTypes.h"
#include "../../PresenterTypes.h"
#include "../../ImageTypes.h"
#include "../../ImageSamplerTypes.h"
#include "../../BufferTypes.h"
#include "../../../ShaderTypes.h"
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief Converts a version provided by Vulkan to a Version struct
	/// @param version The Vulkan version
	/// @return The converted version
	Version ToVersion(uint32_t version);

	/// @brief Converts a Version struct to a Vulkan version
	/// @param version The version
	/// @return The converted Vulkan version
	uint32_t ToVkVersion(const Version& version);

	/// @brief Converts a VkPhysicalDeviceType to a GraphicsDeviceType
	/// @param deviceType The device type
	/// @return The converted device type
	GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType deviceType);

	/// @brief Converts MSAASamples to VkSampleCountFlagBits
	/// @param samples The msaa samples
	/// @return The VkSampleCountFlagBits
	VkSampleCountFlagBits ToVkSampleCountFlagBits(MSAASamples samples);

	/// @brief Converts VkSampleCountFlags to MSAASamples
	/// @param samples The samples
	/// @return The msaa samples
	MSAASamples ToMSAASamples(VkSampleCountFlags samples);

	/// @brief Converts a VSyncMode to a VkPresentModeKHR
	/// @param vsyncMode The v-sync mode
	/// @return The converted present mode
	VkPresentModeKHR ToVkPresentMode(VSyncMode vsyncMode);

	/// @brief Converts a VkPresentModeKHR to a VSyncMode
	/// @param presentMode The present mode
	/// @return The v-sync mode
	VSyncMode ToVSyncMode(VkPresentModeKHR presentMode);

	/// @brief Gets the pixel format of a VkFormat
	/// @param format The format
	/// @return The pixel format
	ImagePixelFormat ToImagePixelFormat(VkFormat format);

	/// @brief Gets the color space of a VkFormat
	/// @param format The format
	/// @return The color space
	ImageColorSpace ToImageColorSpace(VkFormat format);

	/// @brief Converts a VkColorSpaceKHR to an ImageColorSpace
	/// @param colorSpace The color space 
	/// @return The converted color space
	ImageColorSpace ToImageColorSpace(VkColorSpaceKHR colorSpace);

	/// @brief Converts BufferUsageFlags to VkBufferUsageFlags
	/// @param usageFlags The usage flags
	/// @return The VkBufferUsageFlags
	VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usageFlags);

	/// @brief Converts an ImagePixelFormat and ImageColorSpace to a VkFormat
	/// @param pixelFormat The pixel format
	/// @param colorSpace The color space
	/// @return The format
	VkFormat ToVkFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace);

	/// @brief Converts an ImageDimensionType to a VkImageType
	/// @param type The image dimension type
	/// @return The image type
	VkImageType ToVkImageType(ImageDimensionType type);

	/// @brief Convertes usage flags and a pixel format to VkImageUsageFlags
	/// @param usageFlags The image usage flags
	/// @param pixelFormat The image pixel format
	/// @return The image usage flags
	VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags, ImagePixelFormat pixelFormat);

	VkClearValue ToVkClearValue(const Vector4& clearValue, ImagePixelFormat pixelFormat);

	VkImageViewType ToVkImageViewType(ImageDimensionType dimensionType);

	VkImageLayout ToVkImageLayout(ImagePixelFormat pixelFormat);

	/// @brief Converts ShaderStageFlags to VkShaderStageFlags
	/// @param stageFlags The shader stage flags
	/// @return The VkShaderStageFlags
	VkShaderStageFlags ToVkShaderStageFlags(ShaderStageFlags stageFlags);

	/// @brief Converts a ShaderStageType to VkShaderStageFlagBits
	/// @param stage The shader stage
	/// @return The VkShaderStageFlagBits
	VkShaderStageFlagBits ToVkShaderStageFlagBits(ShaderStageType stage);

	/// @brief Converts a PolygonFillMode to a VkPolygonMode
	/// @param fillMode The fill mode
	/// @return The VkPolygonMode
	VkPolygonMode ToVkPolygonMode(PolygonFillMode fillMode);

	/// @brief Converts a CullMode to VkCullModeFlags
	/// @param cullMode The cull mode
	/// @return The VkCullModeFlags
	VkCullModeFlags ToVkCullModeFlags(CullMode cullMode);

	/// @brief Converts a TriangleWindingMode to a VkFrontFace
	/// @param windingMode The winding mode
	/// @return The VkFrontFace
	VkFrontFace ToVkFrontFace(TriangleWindingMode windingMode);

	/// @brief Converts a DepthTestingMode to a VkCompareOp
	/// @param depthMode The depth mode
	/// @return The VkCompareOp
	VkCompareOp ToVkCompareOp(DepthTestingMode depthMode);

	/// @brief Converts a BlendFactorMode to a VkBlendFactor
	/// @param mode The blend factor mode
	/// @return The VkBlendFactor
	VkBlendFactor ToVkBlendFactor(BlendFactorMode mode);

	/// @brief Converts a BlendOperation to a VkBlendOp
	/// @param op The blend operation
	/// @return The VkBlendOp
	VkBlendOp ToVkBlendOp(BlendOperation op);

	/// @brief Converts a BufferDataType to a VkFormat
	/// @param type The data type
	/// @return The VkFormat
	VkFormat ToVkFormat(BufferDataType type);

	/// @brief Converts a TopologyMode to a VkPrimitiveTopology
	/// @param mode The topology mode
	/// @return The VkPrimitiveTopology
	VkPrimitiveTopology ToVkPrimitiveTopology(TopologyMode mode);

	/// @brief Converts an ImageFilterMode to a VkFilter
	/// @param mode The filter mode
	/// @return The VkFilter
	VkFilter ToVkFilter(ImageFilterMode mode);

	/// @brief Converts an ImageRepeatMode to a VkSamplerAddressMode
	/// @param repeatMode The repeat mode
	/// @return The VkSamplerAddressMode
	VkSamplerAddressMode ToVkSamplerAddressMode(ImageRepeatMode repeatMode);

	/// @brief Converts a MipMapFilterMode to a VkSamplerMipmapMode
	/// @param mode The filter mode
	/// @return The VkSamplerMipmapMode
	VkSamplerMipmapMode ToVkSamplerMipmapMode(MipMapFilterMode mode);
}

