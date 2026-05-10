//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_X11WINDOW_H
#define COCOENGINE_X11WINDOW_H
#include <Coco/Windowing/Window.h>
#include <Coco/Core/Types/String.h>
#include "X11Types.h"

namespace Coco
{
    class RenderService;
    class X11WindowSystem;

    /// @brief An implementation of a Window for X11
    class X11Window : public Window
    {
        friend class X11WindowSystem;

    public:
        X11Window(X11WindowSystem* windowSystem, uint32 id, bool mainWindow, const WindowCreateParams& createParams);
        ~X11Window() override;

        void Show() override;
        bool IsVisible() const override { return _isVisible; }
        void SetTitle(const char* title) override;
        String GetTitle() const override;
        void SetSize(const Sizei& newSize) override;
        Sizei GetSize() const override { return _size; }
        void SetPosition(const Vector2i& newPos, bool relativeToParent) override;
        Vector2i GetPosition(bool relativeToParent) const override;
        void SetState(WindowState newState) override;
        WindowState GetState() const override { return _currentState;}
        void SetFullscreen(bool fullscreen) override;
        bool IsFullscreen() const override { return _isFullscreen; }
        void Focus() override;
        bool IsFocused() const override;

        /// @brief Gets the main X11 display (not to be confused with a display monitor)
        /// @return The main X11 display
        X11::Display* GetDisplay() const;

        /// @brief Gets the native window handle
        /// @return The native window handle
        X11::Window GetNativeWindow() const { return _x11Window; }

    protected:
        void Close() override;

    private:
        static const int _eventMask;

        X11WindowSystem* _windowSystem;
        RenderService* _renderService;
        X11::Window _x11Window;
        bool _isMapped;
        bool _isVisible;
        WindowState _currentState;
        bool _isFullscreen;
        Sizei _size;

        /// @brief Gets the string representation of an error code
        /// @param code The error code
        /// @return The string representation of the error code
        String GetErrorText(int code) const;

        /// @brief Processes an event from the X11 server meant for this window
        /// @param event The event
        void ProcessEvent(const X11::XEvent& event);

        /// @brief Updates the current state from the X11 window state
        void UpdateState();

        /// @brief Sets the maximized state of this window
        /// @param maximized If true, this window will be maximized
        void SetMaximizedState(bool maximized);

        /// @brief Sets the fullscreen state of this window
        /// @param fullscreen If true, this window will be fullscreen
        void SetFullscreenState(bool fullscreen);
    };
} // Coco

#endif //COCOENGINE_X11WINDOW_H