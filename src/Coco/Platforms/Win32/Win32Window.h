#pragma once

#include <Coco/Windowing/Window.h>

#include "WindowsIncludes.h"

namespace Coco::Platforms::Win32
{
    using namespace Coco::Windowing;

    /// @brief Win-32 implementation of a Window
    class Win32Window : public Window
    {
        friend class Win32EnginePlatform;

    private:
        HWND _handle;
        string _title;
        bool _canResize;
        bool _isFullscreen;
        bool _decorated;
        bool _cursorVisible;
        WINDOWPLACEMENT _restorePlacement;

    public:
        Win32Window(const WindowCreateParams& createParams);
        ~Win32Window();

        void Show() final;

        void SetTitle(const char* title) final;
        const char* GetTitle() const final { return _title.c_str(); }

        void SetIsFullscreen(bool fullscreen) final;
        bool IsFullscreen() const final { return _isFullscreen; }

        void SetPosition(const Vector2Int& position, bool clientAreaPosition, bool relativeToParent) final;
        Vector2Int GetPosition(bool clientAreaPosition, bool relativeToParent) const final;

        void SetClientAreaSize(const SizeInt& size) final;
        SizeInt GetClientAreaSize() const final;
        void SetSize(const SizeInt& windowSize) final;
        SizeInt GetSize() const final;

        uint16 GetDPI() const final;

        void SetState(WindowState state) final;
        WindowState GetState() const final;

        void Focus() final;
        bool HasFocus() const final;

        bool IsVisible() const final;

        void SetCursorVisibility(bool isVisible) final;
        bool GetCursorVisibility() const final;

        /// @brief Gets this window's internal handle
        /// @return The window handle
        HWND GetHandle() const { return _handle; }

    protected:
        SharedRef<Rendering::GraphicsPresenterSurface> CreateSurface() final;

    private:
        /// @brief Gets window flags for the given options
        /// @param canResize If true, will add window flags for resizing
        /// @param isFullscreen If true, will set flags for a fullscreen window
        /// @param decorated If true, will remove flags for window decoration
        /// @return Window flags
        static DWORD GetWindowFlags(bool canResize, bool isFullscreen, bool decorated);

        /// @brief Gets the outer window size for a given client area
        /// @param clientSize The desired client area
        /// @param windowFlags The window flags
        /// @param exWindowFlags The extended window flags
        /// @return The total window size
        static SizeInt GetAdjustedWindowSize(const SizeInt& clientSize, DWORD windowFlags, DWORD exWindowFlags);

        /// @brief Converts a position from virtual desktop-space to be relative to another window
        /// @param position The position in desktop-space
        /// @param relativeTo The window to convert coordinate space to
        /// @return The position relative to the given window
        static Vector2Int GetRelativePosition(const Vector2Int& position, HWND relativeTo);

        /// @brief Called by the Win32EnginePlatform to process a message for this window
        /// @param message The message
        /// @param wParam The wParam
        /// @param lParam The lParam
        /// @return True if the message was handled
        bool ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

        /// @brief Updates the state of this window
        /// @param state The state for this window to be in
        void UpdateState(WindowState state);

        /// @brief Updates this window to be fullscreen
        /// @param fullscreen If true, this window will become fullscreen
        void UpdateFullscreenState(bool fullscreen);

#ifdef COCO_SERVICES_INPUT
    private:
        /// @brief Handles an input message
        /// @param message The message
        /// @param wParam The wParam
        /// @param lParam The lParam
        /// @return True if the message was handled
        bool HandleInputMessage(UINT message, WPARAM wParam, LPARAM lParam);
#endif
    };
}