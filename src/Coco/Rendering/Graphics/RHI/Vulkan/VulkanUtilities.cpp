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
	VkShaderStageFlags ToVkShaderStageFlags(ShaderStageType stage)
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
			return 0;
		}
	}
}