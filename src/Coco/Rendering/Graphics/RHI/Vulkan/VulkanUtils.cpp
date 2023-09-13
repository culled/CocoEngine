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

	ImagePixelFormat ToImagePixelFormat(VkFormat format)
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
}
