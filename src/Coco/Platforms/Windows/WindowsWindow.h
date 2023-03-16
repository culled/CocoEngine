#pragma once

#include "WindowsIncludes.h"
#include <Coco/Windowing/Window.h>

namespace Coco::Platform::Windows
{
    class EnginePlatformWindows;

    /// <summary>
    /// Win32 implementation of a window
    /// </summary>
    class WindowsWindow final : public Coco::Windowing::Window
    {
    private:
        friend EnginePlatformWindows;

        HWND _handle;
        HINSTANCE _instance;

        SizeInt _size;

    public:
        WindowsWindow(Coco::Windowing::WindowCreateParameters& createParameters, Coco::Windowing::WindowingService* windowingService, EnginePlatformWindows* platform);
        ~WindowsWindow() final;

        WindowsWindow() = delete;
        WindowsWindow(const WindowsWindow&) = delete;
        WindowsWindow& operator=(const WindowsWindow&) = delete;
        WindowsWindow(WindowsWindow&&) = delete;
        WindowsWindow& operator=(WindowsWindow&&) = delete;

        void* GetID() const noexcept final { return _handle; }
        SizeInt GetSize() const noexcept final { return _size; }
        void Show() noexcept final;
        void Minimize() noexcept final;
        bool GetIsVisible() const noexcept final;

    protected:
        void SetupPresenterSurface() final;
        SizeInt GetBackbufferSize() const noexcept final { return GetSize(); }

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

