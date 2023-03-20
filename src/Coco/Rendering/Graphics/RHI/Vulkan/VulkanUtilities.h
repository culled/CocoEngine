#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Version.h>
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>
#include <Coco/Rendering/ShaderTypes.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineTypes.h>
#include <Coco/Rendering/Graphics/Resources/ImageTypes.h>
#include <Coco/Rendering/Graphics/Resources/BufferTypes.h>
#include <Coco/Rendering/Graphics/Resources/ImageSamplerTypes.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// <summary>
	/// Converts a VkPhysicalDeviceType to a GraphicsDeviceType
	/// </summary>
	/// <param name="deviceType">The device type</param>
	/// <returns>The converted device type</returns>
	constexpr GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType deviceType) noexcept
	{
		switch (deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			return GraphicsDeviceType::Discrete;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			return GraphicsDeviceType::Integrated;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			return GraphicsDeviceType::CPU;
		default:
			return GraphicsDeviceType::Other;
		}
	}

	/// <summary>
	/// Converts a version provided by Vulkan to a Version struct
	/// </summary>
	/// <param name="version">The Vulkan version</param>
	/// <returns>The converted version</returns>
	Version ToVersion(uint32_t version) noexcept;

	/// <summary>
	/// Converts a Version struct to a Vulkan version
	/// </summary>
	/// <param name="version">The version</param>
	/// <returns>The converted Vulkan version</returns>
	uint32_t ToVkVersion(const Version& version) noexcept;

	/// <summary>
	/// Converts a VerticalSyncMode to a VkPresentModeKHR
	/// </summary>
	/// <param name="vsyncMode">The vsync mode</param>
	/// <returns>The converted present mode</returns>
	constexpr VkPresentModeKHR ToVkPresentMode(VerticalSyncMode vsyncMode) noexcept
	{
		switch (vsyncMode)
		{
		case VerticalSyncMode::Disabled:
			return VK_PRESENT_MODE_IMMEDIATE_KHR;
		case VerticalSyncMode::Enabled:
			return VK_PRESENT_MODE_FIFO_KHR;
		case VerticalSyncMode::Mailbox:
			return VK_PRESENT_MODE_MAILBOX_KHR;
		default:
			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	/// <summary>
	/// Converts a VkPresentModeKHR to a VerticalSyncMode
	/// </summary>
	/// <param name="presentMode">The present mode</param>
	/// <returns>The converted vertical sync mode</returns>
	constexpr VerticalSyncMode ToVerticalSyncMode(VkPresentModeKHR presentMode) noexcept
	{
		switch (presentMode)
		{
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			return VerticalSyncMode::Disabled;
		case VK_PRESENT_MODE_FIFO_KHR:
			return VerticalSyncMode::Enabled;
		case VK_PRESENT_MODE_MAILBOX_KHR:
			return VerticalSyncMode::Mailbox;
		default:
			return VerticalSyncMode::Enabled;
		}
	}

	/// <summary>
	/// Converts a VkFormat to a PixelFormat
	/// </summary>
	/// <param name="format">The format</param>
	/// <returns>The converted PixelFormat</returns>
	constexpr PixelFormat ToPixelFormat(VkFormat format) noexcept
	{
		switch (format)
		{
		case VK_FORMAT_R8G8B8A8_UNORM:
			return PixelFormat::RGBA8;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			return PixelFormat::Depth24_Stencil8;
		default:
			return PixelFormat::Unknown;
		}
	}

	/// <summary>
	/// Converts a PixelFormat to a VkFormat
	/// </summary>
	/// <param name="format">The format</param>
	/// <returns>The converted VkFormat</returns>
	constexpr VkFormat ToVkFormat(PixelFormat format) noexcept
	{
		switch (format)
		{
		case PixelFormat::RGBA8:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case PixelFormat::Depth24_Stencil8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	/// <summary>
	/// Converts a VkColorSpaceKHR to a ColorSpace
	/// </summary>
	/// <param name="colorSpace">The color space</param>
	/// <returns>The converted ColorSpace</returns>
	constexpr ColorSpace ToColorSpace(VkColorSpaceKHR colorSpace) noexcept
	{
		switch (colorSpace)
		{
		case VK_COLORSPACE_SRGB_NONLINEAR_KHR:
			return ColorSpace::sRGB;
		default:
			return ColorSpace::Unknown;
		}
	}

	/// <summary>
	/// Converts a ColorSpace to a VkColorSpaceKHR
	/// </summary>
	/// <param name="colorSpace">The color space</param>
	/// <returns>The converted VkColorSpaceKHR</returns>
	constexpr VkColorSpaceKHR ToVkColorSpace(ColorSpace colorSpace) noexcept
	{
		switch (colorSpace)
		{
		case ColorSpace::sRGB:
			return VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		default:
			return VK_COLOR_SPACE_MAX_ENUM_KHR;
		}
	}

	/// <summary>
	/// Converts a ShaderStageType to a VkShaderStageFlagBits
	/// </summary>
	/// <param name="stage">The stage</param>
	/// <returns>The converted VkShaderStageFlagBits</returns>
	constexpr VkShaderStageFlagBits ToVkShaderStageFlagBits(ShaderStageType stage) noexcept
	{
		switch (stage)
		{
		case ShaderStageType::Vertex:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderStageType::Tesselation:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderStageType::Geometry:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderStageType::Compute:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
		case ShaderStageType::Fragment:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		default:
			return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		}
	}

	/// <summary>
	/// Converts a PolygonFillMode to a VkPolygonMode
	/// </summary>
	/// <param name="fillMode">The fill mode</param>
	/// <returns>The converted VkPolygonMode</returns>
	constexpr VkPolygonMode ToVkPolygonMode(PolygonFillMode fillMode) noexcept
	{
		switch (fillMode)
		{
		case PolygonFillMode::Point:
			return VK_POLYGON_MODE_POINT;
		case PolygonFillMode::Line:
			return VK_POLYGON_MODE_LINE;
		case PolygonFillMode::Fill:
			return VK_POLYGON_MODE_FILL;
		default:
			return VK_POLYGON_MODE_FILL;
		}
	}

	/// <summary>
	/// Converts a TopologyMode to a VkPrimitiveTopology
	/// </summary>
	/// <param name="topologyMode">The topology mode</param>
	/// <returns>The converted VkPrimitiveTopology</returns>
	constexpr VkPrimitiveTopology ToVkPrimativeTopology(TopologyMode topologyMode) noexcept
	{
		switch (topologyMode)
		{
		case TopologyMode::Points:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case TopologyMode::Lines:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case TopologyMode::LineStrip:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case TopologyMode::Triangles:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case TopologyMode::TriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		default:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	/// <summary>
	/// Converts a CullMode to a VkCullModeFlags
	/// </summary>
	/// <param name="cullMode">The cull mode</param>
	/// <returns>The converted VkCullModeFlags</returns>
	constexpr VkCullModeFlags ToVkCullModeFlags(CullMode cullMode) noexcept
	{
		switch (cullMode)
		{
		case CullMode::Back:
			return VK_CULL_MODE_BACK_BIT;
		case CullMode::Front:
			return VK_CULL_MODE_FRONT_BIT;
		case CullMode::Both:
			return VK_CULL_MODE_FRONT_AND_BACK;
		default:
			return VK_CULL_MODE_NONE;
		}
	}

	/// <summary>
	/// Converts a DepthTestingMode to a VkCompareOp
	/// </summary>
	/// <param name="depthMode">The depth mode</param>
	/// <returns>The converted VkCompareOp</returns>
	constexpr VkCompareOp ToVkCompareOp(DepthTestingMode depthMode) noexcept
	{
		switch (depthMode)
		{
		case DepthTestingMode::LessThan:
			return VK_COMPARE_OP_LESS;
		case DepthTestingMode::LessThanOrEqual:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case DepthTestingMode::Equal:
			return VK_COMPARE_OP_EQUAL;
		case DepthTestingMode::NotEqual:
			return VK_COMPARE_OP_NOT_EQUAL;
		case DepthTestingMode::GreaterThan:
			return VK_COMPARE_OP_GREATER;
		case DepthTestingMode::GreaterThanOrEqual:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case DepthTestingMode::Always:
			return VK_COMPARE_OP_ALWAYS;
		default:
			return VK_COMPARE_OP_NEVER;
		}
	}

	/// <summary>
	/// Converts a BlendOperation to a VkBlendOp
	/// </summary>
	/// <param name="blendOperation">The blend operation</param>
	/// <returns>The converted VkBlendOp</returns>
	constexpr VkBlendOp ToVkBlendOp(BlendOperation blendOperation) noexcept
	{
		switch (blendOperation)
		{
		case BlendOperation::Add:
			return VK_BLEND_OP_ADD;
		case BlendOperation::Subtract:
			return VK_BLEND_OP_SUBTRACT;
		case BlendOperation::ReverseSubtract:
			return VK_BLEND_OP_REVERSE_SUBTRACT;
		case BlendOperation::Min:
			return VK_BLEND_OP_MIN;
		case BlendOperation::Max:
			return VK_BLEND_OP_MAX;
		default:
			return VK_BLEND_OP_ADD;
		}
	}

	/// <summary>
	/// Converts a BlendFactorMode to a VkBlendFactor
	/// </summary>
	/// <param name="blendFactorMode">The blend factor mode</param>
	/// <returns>The converted VkBlendFactor</returns>
	constexpr VkBlendFactor ToVkBlendFactor(BlendFactorMode blendFactorMode) noexcept
	{
		switch (blendFactorMode)
		{
		case BlendFactorMode::Zero:
			return VK_BLEND_FACTOR_ZERO;
		case BlendFactorMode::One:
			return VK_BLEND_FACTOR_ONE;
		case BlendFactorMode::SourceColor:
			return VK_BLEND_FACTOR_SRC_COLOR;
		case BlendFactorMode::OneMinusSourceColor:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case BlendFactorMode::DestinationColor:
			return VK_BLEND_FACTOR_DST_COLOR;
		case BlendFactorMode::OneMinusDestinationColor:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case BlendFactorMode::SourceAlpha:
			return VK_BLEND_FACTOR_SRC_ALPHA;
		case BlendFactorMode::OneMinusSourceAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case BlendFactorMode::DestinationAlpha:
			return VK_BLEND_FACTOR_DST_ALPHA;
		case BlendFactorMode::OneMinusDestinationAlpha:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		default:
			return VK_BLEND_FACTOR_ZERO;
		}
	}

	/// <summary>
	/// Converts a BufferDataFormat to a VkFormat
	/// </summary>
	/// <param name="dataFormat">The buffer data format</param>
	/// <returns>The converted VkFormat</returns>
	constexpr VkFormat ToVkFormat(BufferDataFormat dataFormat) noexcept
	{
		switch (dataFormat)
		{
		case BufferDataFormat::Float:
			return VK_FORMAT_R32_SFLOAT;
		case BufferDataFormat::Int:
			return VK_FORMAT_R32_SINT;
		case BufferDataFormat::Vector2:
			return VK_FORMAT_R32G32_SFLOAT;
		case BufferDataFormat::Vector2Int:
			return VK_FORMAT_R32G32_SINT;
		case BufferDataFormat::Vector3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case BufferDataFormat::Vector3Int:
			return VK_FORMAT_R32G32B32_SINT;
		case BufferDataFormat::Vector4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case BufferDataFormat::Vector4Int:
			return VK_FORMAT_R32G32B32A32_SINT;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	/// <summary>
	/// Converts BufferUsageFlags to VkBufferUsageFlags
	/// </summary>
	/// <param name="usageFlags">The buffer usage flags</param>
	/// <returns>The converted VkBufferUsageFlags</returns>
	VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usageFlags) noexcept;

	/// <summary>
	/// Converts ImageUsageFlags to VkImageUsageFlags
	/// </summary>
	/// <param name="usageFlags">The image usage flags</param>
	/// <returns>The converted VkImageUsageFlags</returns>
	VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags) noexcept;

	/// <summary>
	/// Converts a FilterMode to a VkFilter
	/// </summary>
	/// <param name="filterMode">The filter mode</param>
	/// <returns>The converted VkFilter</returns>
	constexpr VkFilter ToVkFilter(FilterMode filterMode) noexcept
	{
		switch (filterMode)
		{
		case FilterMode::Nearest:
			return VK_FILTER_NEAREST;
		case FilterMode::Linear:
			return VK_FILTER_LINEAR;
		default:
			return VK_FILTER_NEAREST;
		}
	}

	/// <summary>
	/// Converts a FilterMode to a VkSamplerMipmapMode
	/// </summary>
	/// <param name="filterMode">The filter mode</param>
	/// <returns>The converted VkSamplerMipmapMode</returns>
	constexpr VkSamplerMipmapMode ToVkSamplerMipmapMode(FilterMode filterMode) noexcept
	{
		switch (filterMode)
		{
		case FilterMode::Nearest:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		case FilterMode::Linear:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		default:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		}
	}

	/// <summary>
	/// Converts a RepeatMode to a VkSamplerAddressMode
	/// </summary>
	/// <param name="repeatMode">The repeat mode</param>
	/// <returns>The converted VkSamplerAddressMode</returns>
	constexpr VkSamplerAddressMode ToVkSamplerAddressMode(RepeatMode repeatMode) noexcept
	{
		switch (repeatMode)
		{
		case RepeatMode::Clamp:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case RepeatMode::Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case RepeatMode::Mirror:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case RepeatMode::MirrorOnce:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		default:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		}
	}

	/// <summary>
	/// Converts an ImageDimensionType to a VkImageType
	/// </summary>
	/// <param name="dimensionType">The image dimension type</param>
	/// <returns>The converted VkImageType</returns>
	constexpr VkImageType ToVkImageType(ImageDimensionType dimensionType) noexcept
	{
		switch (dimensionType)
		{
		case ImageDimensionType::OneDimensional:
		case ImageDimensionType::OneDimensionalArray:
			return VK_IMAGE_TYPE_1D;
		case ImageDimensionType::TwoDimensional:
		case ImageDimensionType::TwoDimensionalArray:
			return VK_IMAGE_TYPE_2D;
		case ImageDimensionType::ThreeDimensional:
		case ImageDimensionType::ThreeDimensionalArray:
			return VK_IMAGE_TYPE_3D;
		default:
			return VK_IMAGE_TYPE_2D;
		}
	}
}