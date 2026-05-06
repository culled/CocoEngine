//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_X11VULKANGRAPHICSSURFACEFACTORY_H
#define COCOENGINE_X11VULKANGRAPHICSSURFACEFACTORY_H
#include "Coco/Core/Memory/Refs.h"

namespace Coco
{
    class X11Window;
    class VulkanGraphicsPlatform;
    class GraphicsSurface;
    struct VulkanRenderingExtensions;

    class X11VulkanGraphicsSurfaceFactory
    {
    public:
        static Ref<GraphicsSurface> Create(VulkanGraphicsPlatform& graphicsPlatform, const X11Window& window);
        static void GetExtensions(VulkanRenderingExtensions& extensions);
    };
} // Coco

#endif //COCOENGINE_X11VULKANGRAPHICSSURFACEFACTORY_H