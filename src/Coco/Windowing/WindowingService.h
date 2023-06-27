#pragma once

#include <Coco/Core/Services/EngineService.h>

#include <Coco/Core/Types/List.h>
#include "Window.h"

namespace Coco::Windowing
{
    struct WindowCreateParameters;

    /// @brief A service that manages GUI windows
    class COCOAPI WindowingService final : public EngineService
    {
        friend Window;

    private:
        List<ManagedRef<Window>> _windows;
        Ref<Window> _mainWindow;

    public:
        WindowingService(EngineServiceManager* serviceManager);
        ~WindowingService() final;

        /// @brief Creates a window
        /// @param createParameters Parameters to create the new window with
        /// @return The created window
        Ref<Window> CreateNewWindow(WindowCreateParameters& createParameters);

        /// @brief Gets the main window (if one has been created)
        /// @return The main window
        Ref<Window> GetMainWindow() const noexcept { return _mainWindow; }

        /// @brief Attempts to find a window with the given ID
        /// @param windowId The id of the window
        /// @param window Will be filled the window reference if it is found
        /// @return True if a window with the given ID was found
        bool TryFindWindow(void* windowId, Ref<Window>& window) const noexcept;

        List<Ref<Window>> GetRenderableWindows() const noexcept;

    private:
        /// @brief Called when a window has closed
        /// @param window The window that closed
        void WindowClosed(Window* window) noexcept;
    };
}

