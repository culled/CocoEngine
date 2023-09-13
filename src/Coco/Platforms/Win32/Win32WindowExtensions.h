#pragma once

#include <Coco/Core/Types/Refs.h>
#include "Win32Window.h"
#include <Coco/Rendering/Graphics/GraphicsPresenterTypes.h>

namespace Coco::Platforms::Win32
{
    /// @brief Provides window extensions to a Win32EnginePlatform
    class Win32WindowExtensions
    {
    public:
        /// @brief Creates a GraphicsPresenterSurface for a Win32Window
        /// @param renderRHIName The name of the render RHI being used
        /// @param window The window to create the surface for
        /// @return The created surface
        virtual SharedRef<Rendering::GraphicsPresenterSurface> CreateSurfaceForWindow(const char* renderRHIName, const Win32Window& window) const;

    private:
#ifdef COCO_RENDERING_VULKAN
        /// @brief Creates a Vulkan surface for a window
        /// @param window The window to create the surface for
        /// @return The created surface
        SharedRef<Rendering::GraphicsPresenterSurface> CreateVulkanSurfaceForWindow(const Win32Window& window) const;
#endif
    };
}

