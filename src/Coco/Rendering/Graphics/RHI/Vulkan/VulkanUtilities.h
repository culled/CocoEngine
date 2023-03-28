#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Types/Version.h>
#include <Coco/Rendering/ShaderTypes.h>
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineTypes.h>
#include <Coco/Rendering/Graphics/Resources/ImageTypes.h>
#include <Coco/Rendering/Graphics/Resources/BufferTypes.h>
#include <Coco/Rendering/Graphics/Resources/ImageSamplerTypes.h>
#include <Coco/Rendering/Graphics/Resources/GraphicsPresenterTypes.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering::Vulkan
{
	/// @brief Converts a VkPhysicalDeviceType to a GraphicsDeviceType
	/// @param deviceType The device type
	/// @return The converted device type
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

	/// @brief Converts a version provided by Vulkan to a Version struct
	/// @param version The Vulkan version
	/// @return The converted version
	Version ToVersion(uint32_t version) noexcept;

	/// @brief Converts a Version struct to a Vulkan version
	/// @param version The version
	/// @return The converted Vulkan version
	uint32_t ToVkVersion(const Version& version) noexcept;

	/// @brief Converts a VerticalSyncMode to a VkPresentModeKHR
	/// @param vsyncMode The vsync mode
	/// @return The converted VkPresentModeKHR
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

	/// @brief Converts a VkPresentModeKHR to a VerticalSyncMode
	/// @param presentMode The present mode
	/// @return The converted VerticalSyncMode
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

	/// @brief Converts a VkFormat to a PixelFormat
	/// @param format The format
	/// @return The converted PixelFormat
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

	/// @brief Converts a PixelFormat to a VkFormat
	/// @param format The format
	/// @return The converted VkFormat
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

	/// @brief Converts a VkColorSpaceKHR to a ColorSpace
	/// @param colorSpace The color space
	/// @return The converted ColorSpace
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

	/// @brief Converts a ColorSpace to a VkColorSpaceKHR
	/// @param colorSpace The color space
	/// @return The converted VkColorSpaceKHR
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

	/// @brief Converts a ShaderStageType to a VkShaderStageFlagBits
	/// @param stage The stage 
	/// @return The converted VkShaderStageFlagBits
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

	/// @brief Converts a PolygonFillMode to a VkPolygonMode
	/// @param fillMode The fill mode
	/// @return The converted VkPolygonMode
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

	/// @brief Converts a TopologyMode to a VkPrimitiveTopology
	/// @param topologyMode The topology mode
	/// @return The converted VkPrimitiveTopology
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

	/// @brief Converts a CullMode to a VkCullModeFlags
	/// @param cullMode The cull mode 
	/// @return The converted VkCullModeFlags
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

	/// @brief Converts a DepthTestingMode to a VkCompareOp
	/// @param depthMode The depth mode
	/// The converted VkCompareOp
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

	/// @brief Converts a BlendOperation to a VkBlendOp
	/// @param blendOperation The blend operation 
	/// @return The converted VkBlendOp
	constexpr VkBlendOp ToVkBlendOp(BlendOperation blendOperation) noexcept
	{
		switch (blendOperation)
		{
		case BlendOperation::None:
			return VK_BLEND_OP_ADD;
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

	/// @brief Converts a BlendFactorMode to a VkBlendFactor
	/// @param blendFactorMode The blend factor mode
	/// @return The converted VkBlendFactor
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

	/// @brief Converts a BufferDataFormat to a VkFormat
	/// @param dataFormat The buffer data format
	/// @return The converted VkFormat
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

	/// @brief Converts BufferUsageFlags to VkBufferUsageFlags
	/// @param usageFlags The buffer usage flags
	/// @return The converted VkBufferUsageFlags
	VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usageFlags) noexcept;

	/// @brief Converts ImageUsageFlags to VkImageUsageFlags
	/// @param usageFlags The image usage flags
	/// @return The converted VkImageUsageFlags
	VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags) noexcept;

	/// @brief Converts a FilterMode to a VkFilter
	/// @param filterMode The filter mode
	/// @return The converted VkFilter
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

	/// @brief Converts a FilterMode to a VkSamplerMipmapMode
	/// @param filterMode The filter mode
	/// @return The converted VkSamplerMipmapMode
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

	/// @brief Converts a RepeatMode to a VkSamplerAddressMode
	/// @param repeatMode The repeat mode
	/// @return The converted VkSamplerAddressMode
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

	/// @brief Converts an ImageDimensionType to a VkImageType
	/// @param dimensionType The image dimension type
	/// @return The converted VkImageType
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

	constexpr VkImageLayout ToAttachmentLayout(PixelFormat format)
	{
		return IsDepthStencilFormat(format) ?
			VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}
}