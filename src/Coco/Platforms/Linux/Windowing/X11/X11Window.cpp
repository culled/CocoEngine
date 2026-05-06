//
// Created by cullen on 2/28/26.
//

#include "X11Window.h"
#include "X11WindowSystem.h"
#include <Coco/Core/Application.h>

#include "Coco/Rendering/RenderService.h"
#include <Coco/Core/Engine.h>

#include "X11Includes.h"

namespace Coco
{
    const int X11Window::_eventMask =
        ButtonPressMask |
        ButtonReleaseMask |
        EnterWindowMask |
        LeaveWindowMask |
        PointerMotionMask |
        FocusChangeMask |
        KeyPressMask |
        KeyReleaseMask |
        SubstructureNotifyMask |
        StructureNotifyMask |
        PropertyChangeMask;

    X11Window::X11Window(X11WindowSystem* windowSystem, uint32 id, bool mainWindow, const WindowCreateParams& createParams) :
        Window(id, mainWindow, createParams.ParentWindow),
        _windowSystem(windowSystem),
        _renderService(windowSystem->GetEngine()->GetService<RenderService>()),
        _x11Window(),
        _isMapped(false),
        _isVisible(false),
        _currentState(createParams.InitialState),
        _isFullscreen(createParams.IsFullscreen),
        _size(createParams.InitialSize)
    {
        if (!_renderService)
            throw Exception("No active RenderService found");

        GraphicsPlatform* graphicsPlatform = _renderService->GetGraphicsPlatform();
        if (!graphicsPlatform)
            throw Exception("GraphicsPlatform has not been created");

        X11::Window parentWindow = {};
        int x = 0;
        int y = 0;

        if (createParams.InitialPosition)
        {
            x = createParams.InitialPosition->X();
            y = createParams.InitialPosition->Y();
        }

        if (_parent)
        {
            // Get the coordinates relative to the parent
            parentWindow = static_cast<const X11Window&>(*_parent)._x11Window;
            X11::Window w;
            int relX, relY;
            X11::XTranslateCoordinates(_windowSystem->GetMainDisplay(), _windowSystem->GetRootWindow(), parentWindow, x, y, &relX, &relY, &w);
            x = relX;
            y = relY;
        }
        else
        {
            parentWindow = _windowSystem->GetRootWindow();
        }

        X11::XSetWindowAttributes windowAttributes = {};
        windowAttributes.event_mask = _eventMask;
        windowAttributes.bit_gravity = StaticGravity; // This seems to help prevent flickers during window resizing
        windowAttributes.background_pixel = 0;

        _x11Window = X11::XCreateWindow(_windowSystem->GetMainDisplay(), parentWindow,
            x, y,
            createParams.InitialSize.Width, createParams.InitialSize.Height,
            1,
            CopyFromParent,
            CopyFromParent,
            CopyFromParent,
            CWEventMask | CWBitGravity | CWBackPixel,
            &windowAttributes);
        //unsigned long blackPixel = X11::XBlackPixel(_windowSystem->GetMainDisplay(), X11::XDefaultScreen(_windowSystem->GetMainDisplay()));
        //_x11Window = X11::XCreateSimpleWindow(_windowSystem->GetMainDisplay(), parentWindow,
        //    x, y,
        //    createParams.InitialSize.Width, createParams.InitialSize.Height,
        //    0,
        //    blackPixel, blackPixel);
        X11::XSelectInput(_windowSystem->GetMainDisplay(), _x11Window, _eventMask);

        X11Window::SetTitle(createParams.Title);

        // Enable notification when the "X" button on the window is pressed
        X11::XSetWMProtocols(_windowSystem->GetMainDisplay(), _x11Window, &X11WindowSystem::WM_DELETE_WINDOW, 1);

        // Show on top of the parent
        if (_parent)
            X11::XSetTransientForHint(_windowSystem->GetMainDisplay(), _x11Window, parentWindow);

        int32 statesCount = 0;
        X11::Atom states[3];

        if (_isFullscreen)
        {
            states[statesCount++] = X11WindowSystem::NET_WM_STATE_FULLSCREEN;
        }

        if (_currentState == WindowState::Maximized)
        {
            states[statesCount++] = X11WindowSystem::NET_WM_STATE_MAXIMIZED_HORZ;
            states[statesCount++] = X11WindowSystem::NET_WM_STATE_MAXIMIZED_VERT;
        }
        else if (_currentState == WindowState::Minimized)
        {
            states[statesCount++] = X11WindowSystem::NET_WM_STATE_HIDDEN;
        }

        if (statesCount > 0)
        {
            X11::XChangeProperty(_windowSystem->GetMainDisplay(), _x11Window, X11WindowSystem::NET_WM_STATE, 4, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(states), statesCount);
        }

        _surface = _windowSystem->CreateWindowSurface(*graphicsPlatform, *this);

        X11::XFlush(_windowSystem->GetMainDisplay());

        COCO_ENGINE_LOG_VERBOSE("Created X11Window %u", _id);
    }

    X11Window::~X11Window()
    {
        try
        {
            OnClosed.Invoke();
        }
        catch (...)
        {}

        _renderService->RemoveFinalRenderTarget(GetID());

        if (auto platform = _renderService->GetGraphicsPlatform())
        {
            platform->InvalidateResource(_surface->GetID());
        }

        COCO_ENGINE_LOG_VERBOSE("Destroyed X11Window %u", _id);
    }

    void X11Window::Show()
    {
        X11::XMapWindow(_windowSystem->GetMainDisplay(), _x11Window);

        if (_currentState == WindowState::Minimized)
        {
            // HACK: window doesn't seem to respect the hidden state hint when first showing, so minimize after showing
            _currentState = WindowState::Default;
            SetState(WindowState::Minimized);
        }
    }

    void X11Window::SetTitle(const char* title)
    {
        int result = X11::XStoreName(_windowSystem->GetMainDisplay(), _x11Window, title);
        if (result == BadAlloc || result == BadWindow)
            COCO_ENGINE_LOG_ERROR("Error renaming window (code %d): %s", result, GetErrorText(result).CStr());
    }

    String X11Window::GetTitle() const
    {
        char* title = nullptr;
        X11::XFetchName(_windowSystem->GetMainDisplay(), _x11Window, &title);

        String returnTitle(title);
        X11::XFree(title);

        return returnTitle;
    }

    void X11Window::SetSize(const Sizei& newSize)
    {
        COCO_ASSERT(newSize.Width > 0, "Width must be larger than 0");
        COCO_ASSERT(newSize.Height > 0, "Height must be larger than 0");

        int result = X11::XResizeWindow(_windowSystem->GetMainDisplay(), _x11Window, newSize.Width, newSize.Height);
        if (result == BadValue)
            COCO_ENGINE_LOG_ERROR("Error resizing window (code %d): %s", result, GetErrorText(result).CStr());
    }

    /*Sizei X11Window::GetSize() const
    {
        X11::XWindowAttributes windowAttributes = {};
        X11::XGetWindowAttributes(_windowSystem->GetMainDisplay(), _x11Window, &windowAttributes);
        return Sizei(windowAttributes.width, windowAttributes.height);
    }*/

    void X11Window::SetPosition(const Vector2i& newPos, bool relativeToParent)
    {
        int x = newPos.X();
        int y = newPos.Y();

        // Moving a window happens relative to the screen, so we need to map the relative coordinates to screen coordinates if we have a parent
        if (relativeToParent && _parent)
        {
            X11::Window w;
            X11::XTranslateCoordinates(_windowSystem->GetMainDisplay(), _windowSystem->GetRootWindow(), _x11Window,
                newPos.X(), newPos.Y(),
                &x, &y, &w);
        }

        X11::XMoveWindow(_windowSystem->GetMainDisplay(), _x11Window, x, y);
    }

    Vector2i X11Window::GetPosition(bool relativeToParent) const
    {
        if (relativeToParent && _parent)
        {
            X11::XWindowAttributes windowAttributes = {};
            X11::XGetWindowAttributes(_windowSystem->GetMainDisplay(), _x11Window, &windowAttributes);
            return Vector2i(windowAttributes.x, windowAttributes.y);
        }
        else
        {
            int x, y;
            X11::Window w;
            XTranslateCoordinates(_windowSystem->GetMainDisplay(), _x11Window, _windowSystem->GetRootWindow(),
                0, 0,
                &x, &y, &w);

            return Vector2i(x, y);
        }
    }

    void X11Window::SetState(WindowState newState)
    {
        if (newState == _currentState)
            return;

        switch (newState)
        {
            case WindowState::Maximized:
            {
                SetMaximizedState(true);
                break;
            }
            case WindowState::Minimized:
            {
                X11::XIconifyWindow(_windowSystem->GetMainDisplay(), _x11Window, 0);
                break;
            }
            case WindowState::Default:
            {
                if (_currentState == WindowState::Minimized)
                {
                    X11::XMapWindow(_windowSystem->GetMainDisplay(), _x11Window);
                }
                else
                {
                    SetMaximizedState(false);
                }
                break;
            }
        }
    }

    void X11Window::SetFullscreen(bool fullscreen)
    {
        if (fullscreen == _isFullscreen)
            return;

        SetFullscreenState(fullscreen);
    }

    void X11Window::Focus()
    {
        X11::XSetInputFocus(_windowSystem->GetMainDisplay(), _x11Window, RevertToPointerRoot, CurrentTime);
    }

    bool X11Window::IsFocused() const
    {
        X11::Window focusedWindow;
        int revertFocusState;
        X11::XGetInputFocus(_windowSystem->GetMainDisplay(), &focusedWindow, &revertFocusState);
        return focusedWindow == _x11Window;
    }

    X11::Display* X11Window::GetDisplay() const
    {
        return _windowSystem->GetMainDisplay();
    }

    void X11Window::Close()
    {
        if (!_x11Window)
            return;

        if (_isMainWindow)
            Engine::Get()->GetApplication()->Quit();

        X11::XDestroyWindow(_windowSystem->GetMainDisplay(), _x11Window);

        X11::XSync(_windowSystem->GetMainDisplay(), false);
        _windowSystem->ProcessEvents();
    }

    String X11Window::GetErrorText(int code) const
    {
        String text;
        text.Reserve(256);
        X11::XGetErrorText(_windowSystem->GetMainDisplay(), code, text.Data(), static_cast<int>(text.GetCapacity()));
        return text;
    }

    void X11Window::ProcessEvent(const X11::XEvent& event)
    {
        switch (event.type)
        {
            case MapNotify: // A window became visible, either from being shown the first time or unminimized
                _isMapped = true;
                break;
            case UnmapNotify: // A window became invisible (most likely being minimized)
            {
                _isMapped = false;
                _isVisible = false;
                OnVisibilityChanged.Invoke(false);
                break;
            }
            case FocusIn: // A window was focused
                OnFocusChanged.Invoke(true);
                break;
            case FocusOut: // A window lost focus
                OnFocusChanged.Invoke(false);
                break;
            case ResizeRequest: // A window is being resized
            {
                Sizei newSize(event.xresizerequest.width, event.xresizerequest.height);
                _surface->SetFramebufferSize(newSize);
                OnSizeChanged.Invoke(newSize);
                break;
            }
            case ConfigureNotify: // Called when the window moves or is resized when not using ResizeRedirectMask
            {
                Vector2i windowPos(event.xconfigure.x, event.xconfigure.y);
                OnPositionChanged.Invoke(windowPos);

                Sizei newSize(event.xconfigure.width, event.xconfigure.height);
                if (_size != newSize)
                {
                    _size = newSize;
                    _surface->SetFramebufferSize(newSize);
                    OnSizeChanged.Invoke(newSize);
                }
                break;
            }
            case EnterNotify: // Called when the mouse enters the window area
                //COCO_ENGINE_LOG_VERBOSE("EnterNotify");
                break;
            case LeaveNotify: // Called when the mouse leaves the window area
                //COCO_ENGINE_LOG_VERBOSE("LeaveNotify");
                break;
            case PropertyNotify:
                if (event.xproperty.atom == X11WindowSystem::NET_WM_STATE)
                {
                    UpdateState();

                    bool isVisible = _currentState != WindowState::Minimized && _isMapped;
                    if (isVisible != _isVisible)
                    {
                        _isVisible = isVisible;
                        OnVisibilityChanged.Invoke(_isVisible);

                        if (_isVisible)
                            _renderService->AddFinalRenderTarget(GetID(), _surface);
                        else
                            _renderService->RemoveFinalRenderTarget(GetID());
                    }
                }
                break;
            default:
                break;
        }
    }

    void X11Window::UpdateState()
    {
        WindowState windowState = WindowState::Default;
        bool isFullscreen = false;

        // https://iifx.dev/en/articles/460415764/a-guide-to-ewmh-monitoring-window-minimized-restored-states-in-xlib
        // https://code-examples.net/en/q/4c21bf4/a-guide-to-window-state-notifications-for-x11-developers
        X11::Atom propertyType;
        int actualFormat;
        unsigned long itemCount, bytesAfter;
        unsigned char* prop = nullptr;
        X11::XGetWindowProperty(_windowSystem->GetMainDisplay(), _x11Window, X11WindowSystem::NET_WM_STATE, 0, LONG_MAX, false, AnyPropertyType, &propertyType, &actualFormat, &itemCount, &bytesAfter, &prop);

        if (prop)
        {
            X11::Atom* atoms = reinterpret_cast<X11::Atom*>(prop);
            for (unsigned long i = 0; i < itemCount; i++)
            {
                if (atoms[i] == X11WindowSystem::NET_WM_STATE_HIDDEN)
                {
                    windowState = WindowState::Minimized;
                }
                else if (atoms[i] == X11WindowSystem::NET_WM_STATE_MAXIMIZED_VERT)
                {
                    if (windowState != WindowState::Minimized)
                        windowState = WindowState::Maximized;
                }
                else if (atoms[i] == X11WindowSystem::NET_WM_STATE_FULLSCREEN)
                {
                    isFullscreen = true;
                }
            }
            /*if (prop != nullptr)
            {
                unsigned long state = static_cast<unsigned long>(*prop);
                COCO_ENGINE_LOG_VERBOSE("Property: %d", state);
                if (state == IconicState)
                {
                    COCO_ENGINE_LOG_VERBOSE("Minimized state");
                }
                else if (state == NormalState)
                {
                    COCO_ENGINE_LOG_VERBOSE("Default state");
                }
            }*/

            X11::XFree(prop);
        }

        if (_currentState != windowState)
        {
            _currentState = windowState;
            OnStateChanged.Invoke(_currentState);
        }

        _isFullscreen = isFullscreen;
    }

    void X11Window::SetMaximizedState(bool maximized)
    {
        // https://specifications.freedesktop.org/wm/latest-single/#id-1.6
        long state = maximized ? 1l : 0l;// _NET_WM_STATE_ADD or _NET_WM_STATE_REMOVE

        X11::XEvent event = {};
        event.type = ClientMessage;
        event.xclient.window = _x11Window;
        event.xclient.message_type = X11WindowSystem::NET_WM_STATE;
        event.xclient.format = 32;
        event.xclient.data.l[0] = state;
        event.xclient.data.l[1] = static_cast<long>(X11WindowSystem::NET_WM_STATE_MAXIMIZED_VERT);
        event.xclient.data.l[2] = static_cast<long>(X11WindowSystem::NET_WM_STATE_MAXIMIZED_HORZ);
        event.xclient.data.l[3] = 0l;
        X11::XSendEvent(_windowSystem->GetMainDisplay(), _windowSystem->GetRootWindow(), false, SubstructureNotifyMask, &event);
    }

    void X11Window::SetFullscreenState(bool fullscreen)
    {
        long state = fullscreen ? 1l : 0l;// _NET_WM_STATE_ADD or _NET_WM_STATE_REMOVE

        X11::XEvent event = {};
        event.type = ClientMessage;
        event.xclient.window = _x11Window;
        event.xclient.message_type = X11WindowSystem::NET_WM_STATE;
        event.xclient.format = 32;
        event.xclient.data.l[0] = state;
        event.xclient.data.l[1] = static_cast<long>(X11WindowSystem::NET_WM_STATE_FULLSCREEN);
        event.xclient.data.l[2] = 0l;
        event.xclient.data.l[3] = 0l;
        X11::XSendEvent(_windowSystem->GetMainDisplay(), _windowSystem->GetRootWindow(), false, SubstructureNotifyMask, &event);
    }
} // Coco