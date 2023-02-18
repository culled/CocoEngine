#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Services/EngineService.h>

namespace Coco
{
    class Engine;
}

namespace Coco::Windowing
{
    struct WindowCreateParameters;
    class Window;

    /// <summary>
    /// A service that manages GUI windows
    /// </summary>
    class COCOAPI WindowingService : public EngineService
    {
    private:
        friend Window;

        List<Managed<Window>> _windows;
        Window* _mainWindow;

    public:
        WindowingService();
        virtual ~WindowingService() override;

        virtual Logging::Logger* GetLogger() const override;
        virtual void Start() override;

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
        bool TryFindWindow(void* windowId, Window** window) const;

    private:
        /// <summary>
        /// Called when a window has closed
        /// </summary>
        /// <param name="window">The window that closed</param>
        void WindowClosed(Window* window);
    };
}

