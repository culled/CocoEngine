#pragma once

#include "WindowsIncludes.h"
#include <Coco/Windowing/Window.h>

namespace Coco::Windowing
{
    class WindowingService;
}

namespace Coco::Platform::Windows
{
    class EnginePlatformWindows;

    /// <summary>
    /// Win32 implementation of a window
    /// </summary>
    class WindowsWindow : public Coco::Windowing::Window
    {
    private:
        friend EnginePlatformWindows;

        HWND _handle;

    public:
        WindowsWindow(Coco::Windowing::WindowCreateParameters& createParameters, Coco::Windowing::WindowingService* windowingService, EnginePlatformWindows* platform);
        virtual ~WindowsWindow() override;

        virtual void* GetID() const { return _handle; }
        virtual void Show() override;
        virtual void Hide() override;

    private:
        /// <summary>
        /// Called by the EnginePlatformWindows to process a message intended for this window
        /// </summary>
        /// <param name="message">The type of message</param>
        /// <param name="wParam">Message WParams</param>
        /// <param name="lParam">Message LParams</param>
        void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
    };
}

