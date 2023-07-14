#pragma once

#include <Coco/Core/Services/EngineService.h>

#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Singleton.h>
#include "Window.h"
#include "DisplayInfo.h"

namespace Coco::Windowing
{
    struct WindowCreateParameters;

    /// @brief A service that manages GUI windows
    class COCOAPI WindowingService final : public EngineService, public Singleton<WindowingService>
    {
        friend Window;

    private:
        List<ManagedRef<Window>> _windows;
        Ref<Window> _mainWindow;

    public:
        WindowingService();
        ~WindowingService() final;

        /// @brief Creates a window
        /// @param createParameters Parameters to create the new window with
        /// @return The created window
        Ref<Window> CreateNewWindow(const WindowCreateParameters& createParameters);

        /// @brief Gets the main window (if one has been created)
        /// @return The main window
        Ref<Window> GetMainWindow() const noexcept { return _mainWindow; }

        /// @brief Attempts to find a window with the given ID
        /// @param windowId The id of the window
        /// @param window Will be filled the window reference if it is found
        /// @return True if a window with the given ID was found
        bool TryFindWindow(void* windowId, Ref<Window>& window) const noexcept;

        /// @brief Gets a list of windows that are currently visible
        /// @return A list of visible windows
        List<Ref<Window>> GetVisibleWindows() const noexcept;

        /// @brief Gets info for all available displays
        /// @return Info for all available displays
        List<DisplayInfo> GetDisplays() const;

    private:
        /// @brief Called when a window has closed
        /// @param window The window that closed
        void WindowClosed(Window* window) noexcept;
    };
}

