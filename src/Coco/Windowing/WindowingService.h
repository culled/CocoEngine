#pragma once

#include <Coco/Core/Services/EngineService.h>

#include <Coco/Core/Types/List.h>
#include "Window.h"

namespace Coco::Rendering
{
    class RenderingService;
}

namespace Coco::Windowing
{
    struct WindowCreateParameters;

    /// @brief A service that manages GUI windows
    class COCOAPI WindowingService final : public EngineService
    {
        friend Window;

    public:
        /// @brief Priority for the window render tick
        static const int WindowRenderPriority = 1000;

    private:
        List<ManagedRef<Window>> _windows;
        WeakManagedRef<Window> _mainWindow;

        Rendering::RenderingService* _renderingService = nullptr;

    public:
        WindowingService(EngineServiceManager* serviceManager);
        ~WindowingService() final;

        /// @brief Creates a window
        /// @param createParameters Parameters to create the new window with
        /// @return The created window
        WeakManagedRef<Window> CreateNewWindow(WindowCreateParameters& createParameters);

        /// @brief Gets the main window (if one has been created)
        /// @return The main window
        WeakManagedRef<Window> GetMainWindow() const noexcept { return _mainWindow; }

        /// @brief Attempts to find a window with the given ID
        /// @param windowId The id of the window
        /// @param window Will be filled the window reference if it is found
        /// @return True if a window with the given ID was found
        bool TryFindWindow(void* windowId, WeakManagedRef<Window>& window) const noexcept;

    protected:
        void StartImpl() final;

    private:
        /// @brief Called when a window has closed
        /// @param window The window that closed
        void WindowClosed(Window* window) noexcept;

        /// @brief Callback for when all windows should render themselves
        /// @param deltaTime The time since the last tick
        void RenderTick(double deltaTime);
    };
}

