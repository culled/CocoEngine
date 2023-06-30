#pragma once

#include <Coco/Windowing/Window.h>

#include "WindowsIncludes.h"

namespace Coco::Platform::Windows
{
    class EnginePlatformWindows;

    /// @brief Win32 implementation of a window
    class WindowsWindow final : public Coco::Windowing::Window
    {
    private:
        friend EnginePlatformWindows;

        HWND _handle;
        HINSTANCE _instance;

        SizeInt _size;

    public:
        WindowsWindow(const Coco::Windowing::WindowCreateParameters& createParameters, Coco::Windowing::WindowingService* windowingService, EnginePlatformWindows* platform);
        ~WindowsWindow() final;

        WindowsWindow() = delete;
        WindowsWindow(const WindowsWindow&) = delete;
        WindowsWindow& operator=(const WindowsWindow&) = delete;
        WindowsWindow(WindowsWindow&&) = delete;
        WindowsWindow& operator=(WindowsWindow&&) = delete;

        void* GetID() const noexcept final { return _handle; }
        string GetTitle() const noexcept final;
        SizeInt GetSize() const noexcept final { return _size; }
        void Show() noexcept final;
        void Minimize() noexcept final;
        bool GetIsVisible() const noexcept final;

    protected:
        void SetupPresenterSurface() final;
        SizeInt GetBackbufferSize() const noexcept final { return GetSize(); }

    private:
        /// @brief Called by the EnginePlatformWindows to process a message intended for this window
        /// @param message The type of message
        /// @param wParam Message WParams
        /// @param lParam Message LParams
        void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
    };
}

