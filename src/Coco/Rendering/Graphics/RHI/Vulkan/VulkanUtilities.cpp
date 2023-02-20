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
}