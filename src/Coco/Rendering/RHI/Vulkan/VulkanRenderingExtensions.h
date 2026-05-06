//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_VULKANRENDERINGEXTENSIONS_H
#define COCOENGINE_VULKANRENDERINGEXTENSIONS_H
#include "Coco/Rendering/RenderingEnginePlatform.h"
#include "Coco/Core/Types/Array.h"

namespace Coco
{
    struct VulkanRenderingExtensions : public PlatformRenderingExtensions
    {
        Array<const char*> InstanceExtensions;
        Array<const char*> DeviceExtensions;
    };
} // Coco

#endif //COCOENGINE_VULKANRENDERINGEXTENSIONS_H