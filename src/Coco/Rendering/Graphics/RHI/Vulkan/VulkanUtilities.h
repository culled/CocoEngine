#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Rendering/Graphics/GraphicsPlatformTypes.h>
#include <Coco/Core/Types/Version.h>
#include "VulkanIncludes.h"

namespace Coco::Rendering
{
	GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType deviceType);
	Version ToVersion(uint32_t version);
}