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

    /// @brief A factory that creates a graphics surface for Vulkan on X11
    class X11VulkanGraphicsSurfaceFactory
    {
    public:
        /// @brief Creates a graphics surface for the given window
        /// @param graphicsPlatform The graphics platform
        /// @param window The window
        /// @return A graphics surface for the window
        static Ref<GraphicsSurface> Create(VulkanGraphicsPlatform& graphicsPlatform, const X11Window& window);

        /// @brief Gets platform-specific rendering extensions
        /// @param extensions The rendering extensions
        static void GetExtensions(VulkanRenderingExtensions& extensions);
    };
} // Coco

#endif //COCOENGINE_X11VULKANGRAPHICSSURFACEFACTORY_H