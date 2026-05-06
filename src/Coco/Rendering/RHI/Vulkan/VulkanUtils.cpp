//
// Created by cullen on 3/22/26.
//

#include <slang.h>
#include "VulkanUtils.h"

// Fixes for macros that define keywords from 3rd party imports
#undef None
#undef Always

namespace Coco
{
    uint32 VulkanUtils::ToVkVersion(const Version& version)
    {
        return VK_MAKE_API_VERSION(0, version.Major, version.Minor, version.Patch);
    }

    Version VulkanUtils::ToVersion(uint32 vulkanVersion)
    {
        return Version(VK_API_VERSION_MAJOR(vulkanVersion), VK_API_VERSION_MINOR(vulkanVersion), VK_API_VERSION_PATCH(vulkanVersion));
    }

    GraphicsDeviceType VulkanUtils::ToGraphicsDeviceType(VkPhysicalDeviceType deviceType)
    {
        switch (deviceType)
        {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                return GraphicsDeviceType::Discrete;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                return GraphicsDeviceType::Integrated;
            default:
                return GraphicsDeviceType::Other;
        }
    }

    MSAASamples VulkanUtils::ToMSAASamples(VkSampleCountFlags sampleCountFlags)
    {
        for (uint8 sampleCount = static_cast<uint8>(MSAASamples::SixtyFour); sampleCount > 1; --sampleCount)
        {
            VkSampleCountFlags flags = 1 << sampleCount;
            if (sampleCountFlags & flags)
                return static_cast<MSAASamples>(sampleCount);
        }

        return MSAASamples::One;
    }

    VSyncMode VulkanUtils::ToVSyncMode(VkPresentModeKHR presentMode)
    {
        switch (presentMode)
        {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                return VSyncMode::Immediate;
            case VK_PRESENT_MODE_FIFO_KHR:
                return VSyncMode::EveryVBlank;
            default:
                COCO_ASSERT(false, "Unsupported present mode");
                return VSyncMode::EveryVBlank;
        }
    }

    ImagePixelFormat VulkanUtils::ToImagePixelFormat(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_R8G8B8A8_SRGB:
            case VK_FORMAT_R8G8B8A8_UNORM:
                return ImagePixelFormat::RGBA8;
            case VK_FORMAT_B8G8R8A8_SRGB:
                return ImagePixelFormat::BGRA8;
            case VK_FORMAT_R32_SINT:
                return ImagePixelFormat::R32_Int;
            case VK_FORMAT_R32G32_SINT:
                return ImagePixelFormat::R32G32_Int;
            case VK_FORMAT_R32G32B32_SINT:
                return ImagePixelFormat::R32G32B32_Int;
            case VK_FORMAT_R32G32B32A32_SINT:
                return ImagePixelFormat::R32G32B32A32_Int;
            case VK_FORMAT_R32_UINT:
                return ImagePixelFormat::R32_UInt;
            case VK_FORMAT_R32G32_UINT:
                return ImagePixelFormat::R32G32_UInt;
            case VK_FORMAT_R32G32B32_UINT:
                return ImagePixelFormat::R32G32B32_UInt;
            case VK_FORMAT_R32G32B32A32_UINT:
                return ImagePixelFormat::R32G32B32A32_UInt;
            case VK_FORMAT_D24_UNORM_S8_UINT:
                return ImagePixelFormat::Depth24_Stencil8;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return ImagePixelFormat::Depth32_Stencil8;
            default:
                return ImagePixelFormat::Unknown;
        }
    }

    VkFormat VulkanUtils::ToVkFormat(ImagePixelFormat format, ImageColorSpace colorSpace)
    {
        switch (format)
        {
            case ImagePixelFormat::RGBA8:
                return colorSpace == ImageColorSpace::sRGB ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
            case ImagePixelFormat::BGRA8:
                return colorSpace == ImageColorSpace::sRGB ? VK_FORMAT_B8G8R8A8_SRGB : VK_FORMAT_B8G8R8A8_UNORM;
            case ImagePixelFormat::R32_Int:
                return VK_FORMAT_R32_SINT;
            case ImagePixelFormat::R32G32_Int:
                return VK_FORMAT_R32G32_SINT;
            case ImagePixelFormat::R32G32B32_Int:
                return VK_FORMAT_R32G32B32_SINT;
            case ImagePixelFormat::R32G32B32A32_Int:
                return VK_FORMAT_R32G32B32A32_SINT;
            case ImagePixelFormat::R32_UInt:
                return VK_FORMAT_R32_UINT;
            case ImagePixelFormat::R32G32_UInt:
                return VK_FORMAT_R32G32_UINT;
            case ImagePixelFormat::R32G32B32_UInt:
                return VK_FORMAT_R32G32B32_UINT;
            case ImagePixelFormat::R32G32B32A32_UInt:
                return VK_FORMAT_R32G32B32A32_UINT;
            case ImagePixelFormat::Depth32_Stencil8:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    ImageColorSpace VulkanUtils::ToImageColorSpace(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_R8G8B8A8_SRGB:
            case VK_FORMAT_B8G8R8A8_SRGB:
                return ImageColorSpace::sRGB;
            case VK_FORMAT_R8G8B8A8_UNORM:
            case VK_FORMAT_B8G8R8A8_UNORM:
            case VK_FORMAT_R32_SINT:
            case VK_FORMAT_R32G32_SINT:
            case VK_FORMAT_R32G32B32_SINT:
            case VK_FORMAT_R32G32B32A32_SINT:
            case VK_FORMAT_R32_UINT:
            case VK_FORMAT_R32G32_UINT:
            case VK_FORMAT_R32G32B32_UINT:
            case VK_FORMAT_R32G32B32A32_UINT:
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return ImageColorSpace::Linear;
            default:
                return ImageColorSpace::Unknown;
        }
    }

    VkImageViewType VulkanUtils::ToVkImageViewType(uint32 height, uint32 depth, uint32 layers)
    {
        if (layers <= 1)
        {
            if (depth <= 1)
            {
                return height > 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_1D;
            }

            return VK_IMAGE_VIEW_TYPE_3D;
        }

        if (depth <= 1)
        {
            return height > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_1D_ARRAY;
        }

        return VK_IMAGE_VIEW_TYPE_3D;
    }

    VkImageType VulkanUtils::ToVkImageType(uint32 height, uint32 depth)
    {
        if (depth <= 1)
        {
            return height > 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_1D;
        }

        return VK_IMAGE_TYPE_3D;
    }

    VkImageUsageFlags VulkanUtils::ToVkImageUsageFlags(ImageUsageFlags usageFlags, ImageAttachmentType attachmentType)
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
            if (attachmentType == ImageAttachmentType::Color)
            {
                flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            }
            else
            {
                flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            }
        }

        return flags;
    }

    VkSampleCountFlagBits VulkanUtils::ToVkSampleFlags(MSAASamples msaaSamples)
    {
        return static_cast<VkSampleCountFlagBits>(1 << static_cast<uint8>(msaaSamples));
    }

    VkImageLayout VulkanUtils::ToVkImageLayout(ImageAttachmentType attachmentType)
    {
        switch (attachmentType)
        {
            case ImageAttachmentType::DepthStencil:
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case ImageAttachmentType::Depth:
                return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            case ImageAttachmentType::Stencil:
                return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
            default:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
    }

    VkClearValue VulkanUtils::ToVkClearValue(const RenderTargetClearValue& value, ImageAttachmentType attachmentType, ImagePixelType pixelType)
    {
        VkClearValue clearValue;
        if (attachmentType == ImageAttachmentType::Color)
        {
            switch (pixelType)
            {
                case ImagePixelType::Int:
                {
                    clearValue.color.int32[0] = value.IntColor[0];
                    clearValue.color.int32[1] = value.IntColor[1];
                    clearValue.color.int32[2] = value.IntColor[2];
                    clearValue.color.int32[3] = value.IntColor[3];
                    break;
                }
                case ImagePixelType::UInt:
                {
                    clearValue.color.uint32[0] = value.UIntColor[0];
                    clearValue.color.uint32[1] = value.UIntColor[1];
                    clearValue.color.uint32[2] = value.UIntColor[2];
                    clearValue.color.uint32[3] = value.UIntColor[3];
                    break;
                }
                default:
                {
                    clearValue.color.float32[0] = value.FloatColor[0];
                    clearValue.color.float32[1] = value.FloatColor[1];
                    clearValue.color.float32[2] = value.FloatColor[2];
                    clearValue.color.float32[3] = value.FloatColor[3];
                    break;
                }
            }
        }
        else
        {
            clearValue.depthStencil.depth = value.DepthStencil.Depth;
            clearValue.depthStencil.stencil = value.DepthStencil.Stencil;
        }

        return clearValue;
    }

    VkImageAspectFlags VulkanUtils::ToVkImageAspectFlags(ImageAttachmentType attachmentType)
    {
        switch (attachmentType)
        {
            case ImageAttachmentType::DepthStencil:
                return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            case ImageAttachmentType::Depth:
                return VK_IMAGE_ASPECT_DEPTH_BIT;
            case ImageAttachmentType::Stencil:
                return VK_IMAGE_ASPECT_STENCIL_BIT;
            default:
                return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    VkDescriptorType VulkanUtils::ToVkDescriptorType(slang::BindingType bindingType)
    {
        switch (bindingType)
        {
            case slang::BindingType::Texture:
                return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case slang::BindingType::ParameterBlock:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case slang::BindingType::CombinedTextureSampler:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            default:
                COCO_ASSERT(false, "Unknown binding type %u", bindingType);
                return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        }
    }

    VkPolygonMode VulkanUtils::ToVkPolygonMode(PolygonFillMode fillMode) noexcept
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

	VkCullModeFlags VulkanUtils::ToVkCullModeFlags(CullMode cullMode) noexcept
	{
		switch(cullMode)
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

	VkFrontFace VulkanUtils::ToVkFrontFace(TriangleWindingMode windingMode) noexcept
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

	VkCompareOp VulkanUtils::ToVkCompareOp(DepthTestingMode depthMode) noexcept
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

	VkBlendFactor VulkanUtils::ToVkBlendFactor(BlendFactorMode mode) noexcept
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

	VkBlendOp VulkanUtils::ToVkBlendOp(BlendOperation op) noexcept
	{
		switch (op)
		{
		case BlendOperation::Subtract:
			return VK_BLEND_OP_SUBTRACT;
		case BlendOperation::ReverseSubtract:
			return VK_BLEND_OP_REVERSE_SUBTRACT;
		case BlendOperation::Min:
			return VK_BLEND_OP_MIN;
		case BlendOperation::Max:
			return VK_BLEND_OP_MAX;
		case BlendOperation::Add:
		case BlendOperation::None:
		default:
			return VK_BLEND_OP_ADD;
		}
	}

    VkPrimitiveTopology VulkanUtils::ToVkPrimitiveTopology(MeshTopologyMode mode) noexcept
    {
        switch (mode)
        {
            case MeshTopologyMode::Points:
                return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case MeshTopologyMode::Lines:
                return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case MeshTopologyMode::LineStrip:
                return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case MeshTopologyMode::TriangleStrip:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case MeshTopologyMode::Triangles:
            default:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        }
    }

    VkFormat VulkanUtils::ToVkFormat(VertexChannel channel)
    {
        switch (channel)
        {
            case VertexChannel::Position:
            case VertexChannel::Normal:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case VertexChannel::Color:
            case VertexChannel::Tangent:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case VertexChannel::UV0:
                return VK_FORMAT_R32G32_SFLOAT;
            default:
                COCO_ASSERT(false, "Unknown vertex channel type");
                return VK_FORMAT_UNDEFINED;
        }
    }

    VkBufferUsageFlags VulkanUtils::ToVkBufferUsageFlags(BufferUsageFlags usageFlags) noexcept
    {
        VkBufferUsageFlags flags = 0;

        if ((usageFlags & BufferUsageFlags::TransferSource) == BufferUsageFlags::TransferSource)
            flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        if ((usageFlags & BufferUsageFlags::TransferDestination) == BufferUsageFlags::TransferDestination)
            flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        if ((usageFlags & BufferUsageFlags::Uniform) == BufferUsageFlags::Uniform)
            flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

        if ((usageFlags & BufferUsageFlags::Index) == BufferUsageFlags::Index)
            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        if ((usageFlags & BufferUsageFlags::Vertex) == BufferUsageFlags::Vertex)
            flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        return flags;
    }

    VkFilter VulkanUtils::ToVkFilter(ImageFilterMode mode) noexcept
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

    VkSamplerAddressMode VulkanUtils::ToVkSamplerAddressMode(ImageRepeatMode repeatMode) noexcept
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

    VkSamplerMipmapMode VulkanUtils::ToVkSamplerMipmapMode(MipMapFilterMode mode) noexcept
    {
        switch (mode)
        {
            case MipMapFilterMode::Nearest:
                return VK_SAMPLER_MIPMAP_MODE_NEAREST;
            case MipMapFilterMode::Linear:
            default:
                return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }
    }

    VkShaderStageFlags VulkanUtils::ToVkShaderStageFlags(ShaderStageType shaderStage)
    {
        switch (shaderStage)
        {
            case ShaderStageType::Vertex:
                return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderStageType::Fragment:
                return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderStageType::Compute:
                return VK_SHADER_STAGE_COMPUTE_BIT;
            case ShaderStageType::Tesselation:
                return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            case ShaderStageType::Geometry:
                return VK_SHADER_STAGE_GEOMETRY_BIT;
            default:
                return 0;
        }
    }

    ShaderStageFlags VulkanUtils::ToShaderStageFlags(VkShaderStageFlags stageFlags)
    {
        ShaderStageFlags flags = ShaderStageFlags::None;

        if ((stageFlags & VK_SHADER_STAGE_VERTEX_BIT) == VK_SHADER_STAGE_VERTEX_BIT)
            flags |= ShaderStageFlags::Vertex;

        if ((stageFlags & VK_SHADER_STAGE_FRAGMENT_BIT) == VK_SHADER_STAGE_FRAGMENT_BIT)
            flags |= ShaderStageFlags::Fragment;

        if ((stageFlags & VK_SHADER_STAGE_COMPUTE_BIT) == VK_SHADER_STAGE_COMPUTE_BIT)
            flags |= ShaderStageFlags::Compute;

        if ((stageFlags & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT) == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
            flags |= ShaderStageFlags::Tesselation;

        if ((stageFlags & VK_SHADER_STAGE_GEOMETRY_BIT) == VK_SHADER_STAGE_GEOMETRY_BIT)
            flags |= ShaderStageFlags::Geometry;

        return flags;
    }
} // Coco