#include "Renderpch.h"
#include "VulkanUtils.h"

#include <Coco/Core/Types/Version.h>

namespace Coco::Rendering::Vulkan
{
	GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType deviceType)
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

	Version ToVersion(uint32_t version)
    {
        return Version(VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version));
    }

    uint32_t ToVkVersion(const Version& version)
    {
        return VK_MAKE_VERSION(version.Major, version.Minor, version.Patch);
    }

	VkFormat ToVkFormat(ImagePixelFormat pixelFormat, ImageColorSpace colorSpace)
	{
		switch (pixelFormat)
		{
		case ImagePixelFormat::RGBA8:
			return colorSpace == ImageColorSpace::sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
		case ImagePixelFormat::Depth32_Stencil8:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	ImagePixelFormat GetPixelFormat(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_R8G8B8A8_UNORM:
			return ImagePixelFormat::RGBA8;
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return ImagePixelFormat::Depth32_Stencil8;
		default:
			return ImagePixelFormat::Unknown;
		}
	}

	ImageColorSpace GetColorSpace(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_R8G8B8A8_SRGB:
			return ImageColorSpace::sRGB;
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return ImageColorSpace::Linear;
		default:
			return ImageColorSpace::Unknown;
		}
	}

	ImageColorSpace ToImageColorSpace(VkColorSpaceKHR colorSpace)
	{
		switch (colorSpace)
		{
		case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
			return ImageColorSpace::sRGB;
		case VK_COLOR_SPACE_PASS_THROUGH_EXT:
			return ImageColorSpace::Linear;
		default:
			return ImageColorSpace::Unknown;
		}
	}

	VkImageType ToVkImageType(ImageDimensionType type)
	{
		switch (type)
		{
		case ImageDimensionType::TwoDimensional:
		case ImageDimensionType::TwoDimensionalArray:
			return VK_IMAGE_TYPE_2D;
		case ImageDimensionType::ThreeDimensional:
		case ImageDimensionType::ThreeDimensionalArray:
			return VK_IMAGE_TYPE_3D;
		case ImageDimensionType::OneDimensional:
		case ImageDimensionType::OneDimensionalArray:
		default:
			return VK_IMAGE_TYPE_1D;
		}
	}

	VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags, ImagePixelFormat pixelFormat)
	{
		VkImageUsageFlags flags = 0;

		if ((usageFlags & ImageUsageFlags::TransferSource) == ImageUsageFlags::TransferSource)
			flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		if ((usageFlags & ImageUsageFlags::TransferDestination) == ImageUsageFlags::TransferDestination)
			flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		if ((usageFlags & ImageUsageFlags::Sampled) == ImageUsageFlags::Sampled)
			flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

		if ((usageFlags & ImageUsageFlags::RenderTarget) == ImageUsageFlags::RenderTarget)
		{
			if (IsDepthFormat(pixelFormat) || IsStencilFormat(pixelFormat))
			{
				flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			else
			{
				flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
		}

		return flags;
	}

	VkPresentModeKHR ToVkPresentMode(VSyncMode vSyncMode)
	{
		switch (vSyncMode)
		{
		case VSyncMode::Immediate:
			return VK_PRESENT_MODE_IMMEDIATE_KHR;
		case VSyncMode::EveryVBlank:
		default:
			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	VSyncMode ToVSyncMode(VkPresentModeKHR presentMode)
	{
		switch (presentMode)
		{
		case VK_PRESENT_MODE_IMMEDIATE_KHR:
			return VSyncMode::Immediate;
		case VK_PRESENT_MODE_FIFO_KHR:
		default:
			return VSyncMode::EveryVBlank;
		}
	}

	VkImageLayout ToAttachmentLayout(ImagePixelFormat pixelFormat)
	{
		if (IsDepthFormat(pixelFormat) || IsStencilFormat(pixelFormat))
		{
			if (!IsStencilFormat(pixelFormat))
			{
				return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			}
			else if (!IsDepthFormat(pixelFormat))
			{
				return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
			}

			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	VkSampleCountFlagBits ToVkSampleCountFlagBits(MSAASamples samples)
	{
		switch (samples)
		{
		case MSAASamples::Two:
			return VK_SAMPLE_COUNT_2_BIT;
		case MSAASamples::Four:
			return VK_SAMPLE_COUNT_4_BIT;
		case MSAASamples::Eight:
			return VK_SAMPLE_COUNT_8_BIT;
		case MSAASamples::Sixteen:
			return VK_SAMPLE_COUNT_16_BIT;
		case MSAASamples::One:
		default:
			return VK_SAMPLE_COUNT_1_BIT;
		}
	}

	MSAASamples ToMSAASamples(VkSampleCountFlags samples)
	{
		if (samples & VK_SAMPLE_COUNT_16_BIT)
			return MSAASamples::Sixteen;

		if (samples & VK_SAMPLE_COUNT_8_BIT)
			return MSAASamples::Eight;

		if (samples & VK_SAMPLE_COUNT_4_BIT)
			return MSAASamples::Four;

		if (samples & VK_SAMPLE_COUNT_2_BIT)
			return MSAASamples::Two;

		return MSAASamples::One;
	}

	VkBufferUsageFlags ToVkBufferUsageFlags(BufferUsageFlags usageFlags)
	{
		VkBufferUsageFlags flags = 0;

		if ((usageFlags & BufferUsageFlags::TransferSource) == BufferUsageFlags::TransferSource)
			flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		if ((usageFlags & BufferUsageFlags::TransferDestination) == BufferUsageFlags::TransferDestination)
			flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		if ((usageFlags & BufferUsageFlags::UniformTexel) == BufferUsageFlags::UniformTexel)
			flags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::StorageTexel) == BufferUsageFlags::StorageTexel)
			flags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Uniform) == BufferUsageFlags::Uniform)
			flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Storage) == BufferUsageFlags::Storage)
			flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Index) == BufferUsageFlags::Index)
			flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Vertex) == BufferUsageFlags::Vertex)
			flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		if ((usageFlags & BufferUsageFlags::Indirect) == BufferUsageFlags::Indirect)
			flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

		return flags;
	}

	VkShaderStageFlags ToVkShaderStageFlags(ShaderStageFlags stageFlags)
	{
		VkShaderStageFlags flags = 0;

		if ((stageFlags & ShaderStageFlags::Vertex) == ShaderStageFlags::Vertex)
			flags |= VK_SHADER_STAGE_VERTEX_BIT;

		if ((stageFlags & ShaderStageFlags::Tesselation) == ShaderStageFlags::Tesselation)
			flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

		if ((stageFlags & ShaderStageFlags::Geometry) == ShaderStageFlags::Geometry)
			flags |= VK_SHADER_STAGE_GEOMETRY_BIT;

		if ((stageFlags & ShaderStageFlags::Compute) == ShaderStageFlags::Compute)
			flags |= VK_SHADER_STAGE_COMPUTE_BIT;

		if ((stageFlags & ShaderStageFlags::Fragment) == ShaderStageFlags::Fragment)
			flags |= VK_SHADER_STAGE_FRAGMENT_BIT;

		return flags;
	}

	VkShaderStageFlagBits ToVkShaderStageFlagBits(ShaderStageType stage)
	{
		switch (stage)
		{
		case ShaderStageType::Vertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ShaderStageType::Tesselation:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ShaderStageType::Geometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ShaderStageType::Compute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		case ShaderStageType::Fragment:
		default:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		}
	}

	VkPolygonMode ToVkPolygonMode(PolygonFillMode fillMode)
	{
		switch (fillMode)
		{
		case PolygonFillMode::Point:
			return VK_POLYGON_MODE_POINT;
		case PolygonFillMode::Line:
			return VK_POLYGON_MODE_LINE;
		case PolygonFillMode::Fill:
		default:
			return VK_POLYGON_MODE_FILL;
		}
	}

	VkCullModeFlags ToVkCullModeFlags(CullMode cullMode)
	{
		switch (cullMode)
		{
		case CullMode::Back:
			return VK_CULL_MODE_BACK_BIT;
		case CullMode::Front:
			return VK_CULL_MODE_FRONT_BIT;
		case CullMode::Both:
			return VK_CULL_MODE_FRONT_AND_BACK;
		case CullMode::None:
		default:
			return VK_CULL_MODE_NONE;
		}
	}

	VkFrontFace ToVkFrontFace(TriangleWindingMode windingMode)
	{
		switch (windingMode)
		{
		case TriangleWindingMode::Clockwise:
			return VK_FRONT_FACE_CLOCKWISE;
		case TriangleWindingMode::CounterClockwise:
		default:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
	}

	VkCompareOp ToVkCompareOp(DepthTestingMode depthMode)
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
		case DepthTestingMode::Never:
		default:
			return VK_COMPARE_OP_NEVER;
		}
	}

	VkBlendFactor ToVkBlendFactor(BlendFactorMode mode)
	{
		switch (mode)
		{
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
		case BlendFactorMode::Zero:
		default:
			return VK_BLEND_FACTOR_ZERO;
		}
	}

	VkBlendOp ToVkBlendOp(BlendOperation op)
	{
		switch (op)
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
		case BlendOperation::None:
		default:
			return VK_BLEND_OP_ADD;
		}
	}
	VkFormat ToVkFormat(BufferDataType type)
	{
		switch (type)
		{
		case BufferDataType::Float:
			return VK_FORMAT_R32_SFLOAT;
		case BufferDataType::Float2:
			return VK_FORMAT_R32G32_SFLOAT;
		case BufferDataType::Float3:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case BufferDataType::Float4:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case BufferDataType::Int:
			return VK_FORMAT_R32_SINT;
		case BufferDataType::Int2:
			return VK_FORMAT_R32G32_SINT;
		case BufferDataType::Int3:
			return VK_FORMAT_R32G32B32_SINT;
		case BufferDataType::Int4:
			return VK_FORMAT_R32G32B32A32_SINT;
		case BufferDataType::Bool:
			return VK_FORMAT_R8_SINT;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	VkPrimitiveTopology ToVkPrimitiveTopology(TopologyMode mode)
	{
		switch (mode)
		{
		case TopologyMode::Points:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		case TopologyMode::Lines:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case TopologyMode::LineStrip:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		case TopologyMode::TriangleStrip:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		case TopologyMode::Triangles:
		default:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	VkFilter ToVkFilter(ImageFilterMode mode)
	{
		switch (mode)
		{
		case ImageFilterMode::Nearest:
			return VK_FILTER_NEAREST;
		case ImageFilterMode::Linear:
		default:
			return VK_FILTER_LINEAR;
		}
	}

	VkSamplerAddressMode ToVkSamplerAddressMode(ImageRepeatMode repeatMode)
	{
		switch (repeatMode)
		{
		case ImageRepeatMode::Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case ImageRepeatMode::Mirror:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case ImageRepeatMode::MirrorOnce:
			return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
		case ImageRepeatMode::Clamp:
		default:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		}
	}

	VkSamplerMipmapMode ToVkSamplerMipmapMode(MipMapFilterMode mode)
	{
		switch (mode)
		{
		case MipMapFilterMode::Linear:
			return VK_SAMPLER_MIPMAP_MODE_LINEAR;
		case MipMapFilterMode::Nearest:
		default:
			return VK_SAMPLER_MIPMAP_MODE_NEAREST;
		}
	}
}
