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
}
