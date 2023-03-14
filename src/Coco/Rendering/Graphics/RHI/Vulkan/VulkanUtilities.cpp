#include "VulkanUtilities.h"

namespace Coco::Rendering
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
		return Version(VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
	}

	uint32_t ToVkVersion(const Version& version)
	{
		return VK_MAKE_VERSION(version.Major, version.Minor, version.Patch);
	}

	VkPresentModeKHR ToVkPresentMode(VerticalSyncMode vsyncMode)
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

	VerticalSyncMode ToVerticalSyncMode(VkPresentModeKHR presentMode)
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

	PixelFormat ToPixelFormat(VkFormat format)
	{
		switch (format)
		{
		case VK_FORMAT_B8G8R8A8_UNORM:
			return PixelFormat::BGRA8;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			return PixelFormat::Depth24_Stencil8;
		default:
			return PixelFormat::Unknown;
		}
	}

	VkFormat ToVkFormat(PixelFormat format)
	{
		switch (format)
		{
		case PixelFormat::BGRA8:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case PixelFormat::Depth24_Stencil8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	ColorSpace ToColorSpace(VkColorSpaceKHR colorSpace)
	{
		switch (colorSpace)
		{
		case VK_COLORSPACE_SRGB_NONLINEAR_KHR:
			return ColorSpace::sRGB;
		default:
			return ColorSpace::Unknown;
		}
	}

	VkColorSpaceKHR ToVkColorSpace(ColorSpace colorSpace)
	{
		switch (colorSpace)
		{
		case ColorSpace::sRGB:
			return VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		default:
			return VK_COLOR_SPACE_MAX_ENUM_KHR;
		}
	}
	VkShaderStageFlagBits ToVkShaderStageFlagBits(ShaderStageType stage)
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

	VkPolygonMode ToVkPolygonMode(PolygonFillMode fillMode)
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

	VkPrimitiveTopology ToVkPrimativeTopology(TopologyMode topologyMode)
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
		default:
			return VK_CULL_MODE_NONE;
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
		default:
			return VK_COMPARE_OP_NEVER;
		}
	}

	VkBlendOp ToVkBlendOp(BlendOperation blendOperation)
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

	VkBlendFactor ToVkBlendFactor(BlendFactorMode blendFactorMode)
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

	VkFormat ToVkFormat(BufferDataFormat dataFormat)
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

	VkImageUsageFlags ToVkImageUsageFlags(ImageUsageFlags usageFlags)
	{
		VkImageUsageFlags flags = 0;

		if ((usageFlags & ImageUsageFlags::TransferSource) == ImageUsageFlags::TransferSource)
			flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

		if ((usageFlags & ImageUsageFlags::TransferDestination) == ImageUsageFlags::TransferDestination)
			flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		if ((usageFlags & ImageUsageFlags::Sampled) == ImageUsageFlags::Sampled)
			flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

		if ((usageFlags & ImageUsageFlags::RenderTarget) == ImageUsageFlags::RenderTarget)
			flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		return flags;
	}

	VkFilter ToVkFilter(FilterMode filterMode)
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

	VkSamplerMipmapMode ToVkSamplerMipmapMode(FilterMode filterMode)
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

	VkSamplerAddressMode ToVkSamplerAddressMode(RepeatMode repeatMode)
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

	VkImageType ToVkImageType(ImageDimensionType dimensionType)
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

	VkDescriptorType ToVkDescriptorType(ShaderDescriptorType descriptorType)
	{
		switch (descriptorType)
		{
		case ShaderDescriptorType::UniformStruct:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case ShaderDescriptorType::UniformSampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		default:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}
	}
}