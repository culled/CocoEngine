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
        bool _canResize;

        WINDOWPLACEMENT _restorePlacement;

    public:
        WindowsWindow(const Coco::Windowing::WindowCreateParameters& createParameters);
        ~WindowsWindow() final;

        WindowsWindow() = delete;
        WindowsWindow(const WindowsWindow&) = delete;
        WindowsWindow& operator=(const WindowsWindow&) = delete;
        WindowsWindow(WindowsWindow&&) = delete;
        WindowsWindow& operator=(WindowsWindow&&) = delete;

        void* GetID() const noexcept final { return _handle; }
        string GetTitle() const noexcept final;
        void SetTitle(const string& title) final;
        SizeInt GetSize() const noexcept final { return _size; }
        void Show() noexcept final;
        void SetState(Windowing::WindowState newState) final;
        Windowing::WindowState GetState() const noexcept final;
        void SetIsFullscreen(bool isFullscreen) final;
        bool GetIsFullscreen() const final;
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

        /// @brief Gets the flags for a window
        /// @return The flags for a window
        DWORD GetWindowFlags() const;

        /// @brief Updates the state of this window to match the given state
        /// @param newState The state to match
        void UpdateState(Windowing::WindowState newState);

        /// @brief Updates the fullscreen state of this window
        /// @param isFullscreen If true, the window will go fullscreen
        void UpdateFullscreen(bool isFullscreen);
    };
}

