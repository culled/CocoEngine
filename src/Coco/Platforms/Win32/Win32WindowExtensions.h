#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Rendering/Graphics/PresenterTypes.h>

namespace Coco::Platforms::Win32
{
    class Win32Window;

    /// @brief Provides window extensions to a Win32EnginePlatform
    class Win32WindowExtensions
    {
    public:
        /// @brief Creates a GraphicsPresenterSurface for a Win32Window
        /// @param renderRHIName The name of the render RHI being used
        /// @param window The window to create the surface for
        /// @return The created surface
        virtual UniqueRef<Rendering::PresenterSurface> CreateSurfaceForWindow(const string& renderRHIName, const Win32Window& window) const;

        /// @brief Shows an open-file dialog
        /// @param filters The file filters
        /// @return The file path, or empty if no file was selected
        string ShowOpenFileDialog(const std::vector<std::pair<const char*, const char*>>& filters);

        /// @brief Shows an save-file dialog
        /// @param filters The file filters
        /// @return The file path, or empty if no file was selected
        string ShowSaveFileDialog(const std::vector<std::pair<const char*, const char*>>& filters);

    private:
#ifdef COCO_RENDERING_VULKAN
        /// @brief Creates a Vulkan surface for a window
        /// @param window The window to create the surface for
        /// @return The created surface
        UniqueRef<Rendering::PresenterSurface> CreateVulkanSurfaceForWindow(const Win32Window& window) const;
#endif
    };
}

