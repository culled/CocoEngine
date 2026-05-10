//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_WINDOW_H
#define COCOENGINE_WINDOW_H
#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Optional.h>
#include <Coco/Core/Types/Event.h>
#include <Coco/Core/Math/Size.h>
#include <Coco/Core/Math/Vector2.h>
#include <Coco/Core/Memory/Refs.h>
#include "WindowTypes.h"
#include "../Rendering/Graphics/Resources/GraphicsSurface.h"

namespace Coco
{
    class Window;

    /// @brief Parameters for creating a Window
    struct WindowCreateParams
    {
        /// @brief The title of the Window
        const char* Title;

        /// @brief The initial size of the Window
        Sizei InitialSize;

        /// @brief If true, the Window will start fullscreen (defaults to false)
        bool IsFullscreen;

        /// @brief If true, the Window can be resized by the user (defaults to true)
        bool CanResize;

        /// @brief If true, the Window will be focused when it is shown (defaults to true)
        bool FocusWhenShown;

        /// @brief Sets the initial state for the Window (defaults to WindowState::Default)
        WindowState InitialState;

        /// @brief Flags for the Window style (defaults to WindowStyleFlags::None)
        WindowStyleFlags StyleFlags;

        /// @brief If given, will set the initial position for the Window (the top-left point)
        Optional<Vector2i> InitialPosition;

        /// @brief If given, will set the display that the Window opens on
        Optional<uint16> InitialDisplayIndex;

        /// @brief Will set the given Window as the new Window's parent
        Ref<Window> ParentWindow;

        WindowCreateParams(const char* title, const Sizei& initialSize);
    };

    /// @brief Base class for a GUI window
    class Window
    {
    public:
        /// @brief An event that is fired when this window's visibility changes. The parameter will be true if the window became visible
        Event<const bool&> OnVisibilityChanged;

        /// @brief An event that is fired when this window's state changes. The parameter will be the window's new state
        Event<const WindowState&> OnStateChanged;

        /// @brief An event that is fired when this window's size changes. The parameter will be the window's new size
        Event<const Sizei&> OnSizeChanged;

        /// @brief An event that is fired when this window's position changes. The parameter will be the window's new position
        Event<const Vector2i&> OnPositionChanged;

        /// @brief An event that is fired when this window becomes focused or blurred. The parameter will be true if this window became focused
        Event<const bool&> OnFocusChanged;

        /// @brief An event that is fired when the window has been requested to close. Setting the parameter to true will cancel the close request
        Event<bool&> OnCloseRequested;

        /// @brief An event that is fired when the window is closing
        Event<> OnClosed;

        virtual ~Window() = default;

        /// @brief Shows this window, making it become visible
        virtual void Show() = 0;

        /// @brief Determines if this window is visible (not minimized)
        /// @return True if this window is visible
        virtual bool IsVisible() const = 0;

        /// @brief Sets the title of this window
        /// @param title The new window title
        virtual void SetTitle(const char* title) = 0;

        /// @brief Gets the title of this window
        /// @return This window's title
        virtual String GetTitle() const = 0;

        /// @brief Sets the size of this window's client area. NOTE: The window manager may adjust the actual size
        /// @param newSize The new size of the window
        virtual void SetSize(const Sizei& newSize) = 0;

        /// @brief Gets the size of this window's client area
        /// @return The size of this window's client area
        virtual Sizei GetSize() const = 0;

        /// @brief Sets the position of this window
        /// @param newPos The new position of the top-left of the window
        /// @param relativeToParent If true, the position will be relative to this window's parent's position
        virtual void SetPosition(const Vector2i& newPos, bool relativeToParent) = 0;

        /// @brief Gets the position of this window
        /// @param relativeToParent If true, the position will be relative to this window's parent's position
        /// @return The position of the top-left of this window
        virtual Vector2i GetPosition(bool relativeToParent) const = 0;

        /// @brief Sets the state of this window
        /// @param newState The new window state
        virtual void SetState(WindowState newState) = 0;

        /// @brief Gets the state of this window
        /// @return The state of this window
        virtual WindowState GetState() const = 0;

        /// @brief Sets the fullscreen state of this window
        /// @param fullscreen If true, this window will become fullscreen
        virtual void SetFullscreen(bool fullscreen) = 0;

        /// @brief Determines if this window is fullscreen
        /// @return True if this window is currently fullscreen
        virtual bool IsFullscreen() const = 0;

        /// @brief Focuses this window
        virtual void Focus() = 0;

        /// @brief Determines if this window has focus
        /// @return True if this window currently has focus
        virtual bool IsFocused() const = 0;

        /// @brief Gets this window's ID
        /// @return The ID of this window
        uint32 GetID() const { return _id; }

        /// @brief Determines if this window is the main window. The first window created is always the main window, and when the main window closes, the application quits
        /// @return True if this window is the main window
        bool IsMainWindow() const { return _isMainWindow; }

        /// @brief Requests this window to close
        void RequestClose();

        /// @brief Gets the graphics surface of this window that can be used to draw to it
        /// @return The graphics surface of this window
        Ref<GraphicsSurface> GetSurface() { return _surface; }

    protected:
        uint32 _id;
        Ref<Window> _parent;
        bool _isMainWindow;
        Ref<GraphicsSurface> _surface;

        Window(uint32 id, bool isMainWindow, Ref<Window> parentWindow);

        /// @brief Closes this window
        virtual void Close() = 0;
    };
} // Coco

#endif //COCOENGINE_WINDOW_H