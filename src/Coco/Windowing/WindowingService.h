#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/MainLoop/MainLoopTickListener.h>

#include "Window.h"

namespace Coco::Rendering
{
    class RenderingService;
}

namespace Coco::Windowing
{
    struct WindowCreateParameters;

    /// <summary>
    /// A service that manages GUI windows
    /// </summary>
    class COCOAPI WindowingService final : public EngineService
    {
    public:
        static const int WindowRenderPriority = 1000;

    private:
        friend Window;

        List<Managed<Window>> _windows;
        Window* _mainWindow;

        Rendering::RenderingService* _renderingService = nullptr;

    public:
        WindowingService(Coco::Engine* engine);
        ~WindowingService() final;

        /// <summary>
        /// Creates a window
        /// </summary>
        /// <param name="createParameters">Parameters to create the new window with</param>
        /// <returns>The created window</returns>
        Window* CreateNewWindow(WindowCreateParameters& createParameters);

        /// <summary>
        /// Attempts to find a window with the given ID
        /// </summary>
        /// <param name="windowId">The id of the window</param>
        /// <param name="window">The pointer to the window with the given ID if it was found</param>
        /// <returns>True if a window with the given ID was found</returns>
        bool TryFindWindow(void* windowId, Window*& window) const noexcept;

    protected:
        void StartImpl() final;

    private:
        /// <summary>
        /// Called when a window has closed
        /// </summary>
        /// <param name="window">The window that closed</param>
        void WindowClosed(Window* window) noexcept;

        /// <summary>
        /// Callback for when all windows should render themselves
        /// </summary>
        /// <param name="deltaTime">The time since the last tick</param>
        void RenderTick(double deltaTime);
    };
}

