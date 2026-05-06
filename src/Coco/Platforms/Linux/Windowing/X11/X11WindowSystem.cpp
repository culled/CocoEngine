//
// Created by cullen on 2/26/26.
//

#include "X11WindowSystem.h"

#include "X11Window.h"
#include "Coco/Rendering/Graphics/GraphicsPlatform.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"
#include "Vulkan/X11VulkanGraphicsSurfaceFactory.h"

#ifdef COCO_RENDERING_OPENGL
#include "Coco/Rendering/RHI/OpenGL/OpenGLGraphicsPlatform.h"
#include "GraphicsSurfaceFactories/X11OpenGLGraphicsSurfaceFactory.h"
#endif

#ifdef COCO_RENDERING_VULKAN
#include "Coco/Rendering/RHI/Vulkan/VulkanGraphicsPlatform.h"
#include "Coco/Rendering/RHI/Vulkan/VulkanRenderingExtensions.h"
#include "Vulkan/X11VulkanGraphicsSurfaceFactory.h"
#endif

#ifdef COCO_SERVICE_INPUT
#include <Coco/Input/InputService.h>
#endif

#include <Coco/Core/Engine.h>
#include "X11Includes.h"

namespace Coco
{
    X11::Atom X11WindowSystem::WM_DELETE_WINDOW = 0;
    X11::Atom X11WindowSystem::WM_STATE = 0;
    X11::Atom X11WindowSystem::NET_WM_STATE = 0;
    X11::Atom X11WindowSystem::NET_WM_STATE_MAXIMIZED_VERT = 0;
    X11::Atom X11WindowSystem::NET_WM_STATE_MAXIMIZED_HORZ = 0;
    X11::Atom X11WindowSystem::NET_WM_STATE_HIDDEN = 0;
    X11::Atom X11WindowSystem::NET_WM_STATE_FULLSCREEN = 0;

    X11WindowSystem::X11WindowSystem(Engine* engine) :
        LinuxWindowSystem(engine),
        _mainDisplay(nullptr),
        _rootWindow(),
        _nextWindowID(0),
        _activeWindows(),
        _x11WindowToActiveWindow()
    {
        // Connect to XServer
        _mainDisplay = X11::XOpenDisplay(nullptr);
        if (!_mainDisplay)
            throw Exception("Could not open X11 display");

        // Get the default root window of the display, which will serve as the parent of our root window
        _rootWindow = X11::XDefaultRootWindow(_mainDisplay);

        // Setup various Atoms
        WM_DELETE_WINDOW = X11::XInternAtom(_mainDisplay, "WM_DELETE_WINDOW", false);
        WM_STATE = X11::XInternAtom(_mainDisplay, "WM_STATE", false);
        NET_WM_STATE = X11::XInternAtom(_mainDisplay, "_NET_WM_STATE", false);
        NET_WM_STATE_MAXIMIZED_VERT = X11::XInternAtom(_mainDisplay, "_NET_WM_STATE_MAXIMIZED_VERT", false);
        NET_WM_STATE_MAXIMIZED_HORZ = X11::XInternAtom(_mainDisplay, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
        NET_WM_STATE_HIDDEN = X11::XInternAtom(_mainDisplay, "_NET_WM_STATE_HIDDEN", false);
        NET_WM_STATE_FULLSCREEN = X11::XInternAtom(_mainDisplay, "_NET_WM_STATE_FULLSCREEN", false);

        #ifdef COCO_SERVICE_INPUT
        SetupRawInput();
        #endif

        COCO_ENGINE_LOG_VERBOSE("Created X11WindowSystem");
    }

    X11WindowSystem::~X11WindowSystem()
    {
        if (_mainDisplay)
            X11::XCloseDisplay(_mainDisplay);

        COCO_ENGINE_LOG_VERBOSE("Destroyed X11WindowSystem");
    }

    void X11WindowSystem::GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions)
    {
#ifdef COCO_RENDERING_VULKAN
        if (strcmp(graphicsPlatform.GetName(),VulkanGraphicsPlatform::Name) == 0)
        {
            VulkanRenderingExtensions& vulkanExtensions = static_cast<VulkanRenderingExtensions&>(extensions);
            X11VulkanGraphicsSurfaceFactory::GetExtensions(vulkanExtensions);
        }
#endif
    }

    uint16 X11WindowSystem::GetDisplayCount() const
    {
        // Use Xrandr to get monitor count
        int monitorCount = 0;
        X11::XRRMonitorInfo* monitors = X11::XRRGetMonitors(_mainDisplay, _rootWindow, 0, &monitorCount);
        X11::XRRFreeMonitors(monitors);
        return static_cast<uint16>(monitorCount);
    }

    void X11WindowSystem::GetDisplays(Span<DisplayInfo> outDisplays)
    {
        // Use Xrandr to get monitor information
        int monitorCount = 0;
        X11::XRRMonitorInfo* monitors = XRRGetMonitors(_mainDisplay, _rootWindow, 0, &monitorCount);
        X11::XRRMonitorInfo* currentMonitor = monitors;

        COCO_ASSERT(outDisplays.size() == monitorCount, "Display array size did not equal display count");

        for (int i = 0; i < monitorCount; ++i)
        {
            DisplayInfo& displayInfo = outDisplays[i];

            char* displayName = X11::XGetAtomName(_mainDisplay, currentMonitor->name);
            displayInfo.Name = displayName;
            X11::XFree(displayName);

            displayInfo.IsPrimary = currentMonitor->primary == 1;
            displayInfo.Resolution = Sizei(currentMonitor->width, currentMonitor->height);
            float widthInches = static_cast<float>(currentMonitor->mwidth) * 0.03937008f;
            float dpi = static_cast<float>(displayInfo.Resolution.Width) / widthInches;
            displayInfo.DPI = static_cast<uint16>(std::round(dpi));
            displayInfo.Offset = Vector2i(currentMonitor->x, currentMonitor->y);

            ++currentMonitor;
        }

        X11::XRRFreeMonitors(monitors);
    }

    Ref<Window> X11WindowSystem::CreateWindow(const WindowCreateParams& createParams)
    {
        bool isMainWindow = _activeWindows.IsEmpty();
        uint32 windowID = _nextWindowID;

        Ref<X11Window> window = _activeWindows.Emplace(windowID, CreateDefaultManagedRef<X11Window>(this, windowID, isMainWindow, createParams));
        _x11WindowToActiveWindow.Emplace(window->_x11Window, window);

        ++_nextWindowID;

        return window;
    }

    uint64 X11WindowSystem::GetWindowCount(bool includeInactive) const
    {
        uint64 count = 0;

        for (const auto& pair : _activeWindows)
        {
            if (pair.second->IsVisible() || includeInactive)
                ++count;
        }

        return count;
    }

    void X11WindowSystem::GetWindows(bool includeInactive, Span<Ref<Window>> outWindows)
    {
        uint64 index = 0;
        for (auto& pair : _activeWindows)
        {
            if (!pair.second->IsVisible() && !includeInactive)
                continue;

            outWindows[index] = Ref<X11Window>(pair.second);
            ++index;
        }
    }

    void X11WindowSystem::DestroyWindows()
    {
        // Close() will remove the window from the map, so loop and destroy the first element until all windows are gone
        while (!_activeWindows.IsEmpty())
        {
            auto it = _activeWindows.begin();
            it->second->Close();
        }
    }

    void X11WindowSystem::ProcessEvents()
    {
        X11::XEvent event = {};

        // Process all queued events
        while (X11::XPending(_mainDisplay))
        {
            X11::XNextEvent(_mainDisplay, &event);
            ProcessEvent(event);
        }
    }

    Ref<GraphicsSurface> X11WindowSystem::CreateWindowSurface(GraphicsPlatform& graphicsPlatform, X11Window& window)
    {
#ifdef COCO_RENDERING_OPENGL
        if (strcmp(graphicsPlatform.GetName(), OpenGLGraphicsPlatform::Name) == 0)
        {
            return X11OpenGLGraphicsSurfaceFactory::Create(static_cast<OpenGLGraphicsPlatform&>(graphicsPlatform), window);
        }
#endif

#ifdef COCO_RENDERING_VULKAN
        if (strcmp(graphicsPlatform.GetName(), VulkanGraphicsPlatform::Name) == 0)
        {
            return X11VulkanGraphicsSurfaceFactory::Create(static_cast<VulkanGraphicsPlatform&>(graphicsPlatform), window);
        }
#endif

        String str = FormatString("Unsupported graphics platform: %s", graphicsPlatform.GetName());
        throw Exception(str.CStr());
    }

    void X11WindowSystem::ProcessEvent(X11::XEvent& event)
    {
        if (event.xcookie.type == GenericEvent)
        {
            #ifdef COCO_SERVICE_INPUT
            if (event.xcookie.extension == _rawInputOpcode && X11::XGetEventData(_mainDisplay, &event.xcookie))
            {
                ProcessRawInputEvent(event);
                X11::XFreeEventData(_mainDisplay, &event.xcookie);

                return;
            }
            #endif
        }

        switch (event.type)
        {
            case MapNotify: // A window became visible, either from being shown the first time or unminimized
            case UnmapNotify: // A window became invisible (most likely being minimized)
            case FocusIn: // A window was focused
            case FocusOut: // A window lost focus
            case ResizeRequest: // A window is being resized
            case ConfigureNotify: // Called when the window moves or is resized when not using ResizeRedirectMask
            case EnterNotify: // Called when the mouse enters the window area
            case LeaveNotify: // Called when the mouse leaves the window area
            case PropertyNotify: // A window's property changed
            case ClientMessage: // A client message was received (usually WM_DELETE_WINDOW)
            case DestroyNotify: // A window was destroyed
                DispatchWindowEvent(event);
                break;
            case ButtonPress: // A mouse button was pressed
            case ButtonRelease: // A mouse button was released
            case MotionNotify: // Called when the mouse moves in the window area
            case KeyPress: // Called when a key is pressed
            case KeyRelease: // Called when a key is released
                #ifdef COCO_SERVICE_INPUT
                ProcessInputEvent(event);
                #endif
                break;
            default:
                COCO_ENGINE_LOG_VERBOSE("Event type: %d", event.type);
                break;
        }
    }

    void X11WindowSystem::DispatchWindowEvent(const X11::XEvent& event)
    {
        if (!_x11WindowToActiveWindow.Contains(event.xany.window))
        {
            COCO_ENGINE_LOG_VERBOSE("Window %u is not active for event %u", event.xany.window, event.type);
            return;
        }

        // Get the window that the X11 window is linked to
        Ref<X11Window> targetWindow = _x11WindowToActiveWindow[event.xany.window];

        switch (event.type)
        {
            case ClientMessage:
            {
                if (event.xclient.data.l[0] == WM_DELETE_WINDOW) // Called when the close button is pressed
                {
                    // Allow event listeners to interrupt the close (like for an unsaved work popup) by requesting it
                    targetWindow->RequestClose();
                }
                break;
            }
            case DestroyNotify: // A window was destroyed
            {
                // Remove the window from all maps
                _x11WindowToActiveWindow.Remove(event.xany.window);
                _activeWindows.Remove(targetWindow->GetID());
                break;
            }
            default:
                // Forward the event to the window for processing
                targetWindow->ProcessEvent(event);
                break;
        }
    }

    #ifdef COCO_SERVICE_INPUT
    KeyboardKey ToKeyboardKey(uint32 keySym)
    {
        switch (keySym)
        {
            case XK_BackSpace:
                return KeyboardKey::Backspace;
            case XK_Tab:
                return KeyboardKey::Tab;
            case XK_Return:
            case XK_KP_Enter:
                return KeyboardKey::Return;
            case XK_Pause:
                return KeyboardKey::Pause;
            case XK_Caps_Lock:
                return KeyboardKey::CapsLock;
            case XK_Escape:
                return KeyboardKey::Escape;
            case XK_space:
                return KeyboardKey::Space;
            case XK_Page_Up:
                return KeyboardKey::PageUp;
            case XK_Page_Down:
                return KeyboardKey::PageDown;
            case XK_End:
                return KeyboardKey::End;
            case XK_Home:
                return KeyboardKey::Home;
            case XK_Left:
                return KeyboardKey::ArrowLeft;
            case XK_Up:
                return KeyboardKey::ArrowUp;
            case XK_Right:
                return KeyboardKey::ArrowRight;
            case XK_Down:
                return KeyboardKey::ArrowDown;
            case XK_Print:
                return KeyboardKey::Print;
            case XK_Execute:
                return KeyboardKey::Execute;
            case XK_Sys_Req:
                return KeyboardKey::PrintScreen;
            case XK_Insert:
                return KeyboardKey::Insert;
            case XK_Delete:
                return KeyboardKey::Delete;
            case XK_0:
                return KeyboardKey::D0;
            case XK_1:
                return KeyboardKey::D1;
            case XK_2:
                return KeyboardKey::D2;
            case XK_3:
                return KeyboardKey::D3;
            case XK_4:
                return KeyboardKey::D4;
            case XK_5:
                return KeyboardKey::D5;
            case XK_6:
                return KeyboardKey::D6;
            case XK_7:
                return KeyboardKey::D7;
            case XK_8:
                return KeyboardKey::D8;
            case XK_9:
                return KeyboardKey::D9;
            case XK_a:
                return KeyboardKey::A;
            case XK_b:
                return KeyboardKey::B;
            case XK_c:
                return KeyboardKey::C;
            case XK_d:
                return KeyboardKey::D;
            case XK_e:
                return KeyboardKey::E;
            case XK_f:
                return KeyboardKey::F;
            case XK_g:
                return KeyboardKey::G;
            case XK_h:
                return KeyboardKey::H;
            case XK_i:
                return KeyboardKey::I;
            case XK_j:
                return KeyboardKey::J;
            case XK_k:
                return KeyboardKey::K;
            case XK_l:
                return KeyboardKey::L;
            case XK_m:
                return KeyboardKey::M;
            case XK_n:
                return KeyboardKey::N;
            case XK_o:
                return KeyboardKey::O;
            case XK_p:
                return KeyboardKey::P;
            case XK_q:
                return KeyboardKey::Q;
            case XK_r:
                return KeyboardKey::R;
            case XK_s:
                return KeyboardKey::S;
            case XK_t:
                return KeyboardKey::T;
            case XK_u:
                return KeyboardKey::U;
            case XK_v:
                return KeyboardKey::V;
            case XK_w:
                return KeyboardKey::W;
            case XK_x:
                return KeyboardKey::X;
            case XK_y:
                return KeyboardKey::Y;
            case XK_z:
                return KeyboardKey::Z;
            case XK_Meta_L:
                return KeyboardKey::LeftMeta;
            case XK_Meta_R:
                return KeyboardKey::RightMeta;
            case XK_KP_0:
            case XK_KP_Insert:
                return KeyboardKey::Numpad0;
            case XK_KP_1:
            case XK_KP_End:
                return KeyboardKey::Numpad1;
            case XK_KP_2:
            case XK_KP_Down:
                return KeyboardKey::Numpad2;
            case XK_KP_3:
            case XK_KP_Page_Down:
                return KeyboardKey::Numpad3;
            case XK_KP_4:
            case XK_KP_Left:
                return KeyboardKey::Numpad4;
            case XK_KP_5:
            case XK_KP_Begin:
                return KeyboardKey::Numpad5;
            case XK_KP_6:
            case XK_KP_Right:
                return KeyboardKey::Numpad6;
            case XK_KP_7:
            case XK_KP_Home:
                return KeyboardKey::Numpad7;
            case XK_KP_8:
            case XK_KP_Up:
                return KeyboardKey::Numpad8;
            case XK_KP_9:
            case XK_KP_Page_Up:
                return KeyboardKey::Numpad9;
            case XK_KP_Multiply:
                return KeyboardKey::NumpadMultiply;
            case XK_KP_Add:
                return KeyboardKey::NumpadPlus;
            case XK_KP_Separator:
                return KeyboardKey::Separator;
            case XK_KP_Subtract:
                return KeyboardKey::NumpadMinus;
            case XK_KP_Decimal:
            case XK_KP_Delete:
                return KeyboardKey::NumpadDecimal;
            case XK_KP_Divide:
                return KeyboardKey::NumpadDivide;
            case XK_F1:
                return KeyboardKey::F1;
            case XK_F2:
                return KeyboardKey::F2;
            case XK_F3:
                return KeyboardKey::F3;
            case XK_F4:
                return KeyboardKey::F4;
            case XK_F5:
                return KeyboardKey::F5;
            case XK_F6:
                return KeyboardKey::F6;
            case XK_F7:
                return KeyboardKey::F7;
            case XK_F8:
                return KeyboardKey::F8;
            case XK_F9:
                return KeyboardKey::F9;
            case XK_F10:
                return KeyboardKey::F10;
            case XK_F11:
                return KeyboardKey::F11;
            case XK_F12:
                return KeyboardKey::F12;
            case XK_F13:
                return KeyboardKey::F13;
            case XK_F14:
                return KeyboardKey::F14;
            case XK_F15:
                return KeyboardKey::F15;
            case XK_F16:
                return KeyboardKey::F16;
            case XK_F17:
                return KeyboardKey::F17;
            case XK_F18:
                return KeyboardKey::F18;
            case XK_F19:
                return KeyboardKey::F19;
            case XK_F20:
                return KeyboardKey::F20;
            case XK_F21:
                return KeyboardKey::F21;
            case XK_F22:
                return KeyboardKey::F22;
            case XK_F23:
                return KeyboardKey::F23;
            case XK_F24:
                return KeyboardKey::F24;
            case XK_Num_Lock:
                return KeyboardKey::NumLock;
            case XK_Scroll_Lock:
                return KeyboardKey::ScrollLock;
            case XK_Shift_L:
                return KeyboardKey::LeftShift;
            case XK_Shift_R:
                return KeyboardKey::RightShift;
            case XK_Control_L:
                return KeyboardKey::LeftControl;
            case XK_Control_R:
                return KeyboardKey::RightControl;
            case XK_Alt_L:
                return KeyboardKey::LeftAlt;
            case XK_Alt_R:
                return KeyboardKey::RightAlt;
            case XK_semicolon:
                return KeyboardKey::Semicolon;
            case XK_equal:
                return KeyboardKey::Equals;
            case XK_comma:
                return KeyboardKey::Comma;
            case XK_minus:
                return KeyboardKey::Minus;
            case XK_period:
                return KeyboardKey::Period;
            case XK_slash:
                return KeyboardKey::ForwardSlash;
            case XK_grave:
                return KeyboardKey::BackQuote;
            case XK_bracketleft:
                return KeyboardKey::LeftBracket;
            case XK_backslash:
                return KeyboardKey::BackSlash;
            case XK_bracketright:
                return KeyboardKey::RightBracket;
            case XK_apostrophe:
                return KeyboardKey::SingleQuote;

            default:
                COCO_ENGINE_LOG_VERBOSE("Unknown keySym: %u", keySym);
                return KeyboardKey::Unknown;
        }
    }

    void X11WindowSystem::SetupRawInput()
    {
        _rawInputAvailable = false;

        int event, error;
        if (!X11::XQueryExtension(_mainDisplay, "XInputExtension", &_rawInputOpcode, &event, &error))
        {
            COCO_ENGINE_LOG_INFO("XInputExtension not available - raw input will be disabled");
            return;
        }

        int major = 2;
        int minor = 0;
        if (X11::XIQueryVersion(_mainDisplay, &major, &minor) == BadRequest ||
            major < 2)
        {
            COCO_ENGINE_LOG_INFO("XInput2 not available - raw input will be disabled");
            return;
        }

        X11::XIEventMask mask;
        unsigned char bitmask[] = { 0, 0, 0 };
        mask.deviceid = XIAllMasterDevices;
        mask.mask_len = sizeof(bitmask);
        mask.mask = bitmask;

        XISetMask(bitmask, XI_RawMotion);
        X11::XISelectEvents(_mainDisplay, _rootWindow, &mask, 1);

        _rawInputAvailable = true;
    }

    void X11WindowSystem::ProcessRawInputEvent(X11::XEvent& event)
    {
        InputService* input = _engine->GetService<InputService>();
        if (!input)
            return;

        switch (event.xcookie.evtype)
        {
            case XI_RawMotion:
            {
                X11::XIRawEvent* rawEvent = reinterpret_cast<X11::XIRawEvent*>(event.xcookie.data);
                double* rawValue = rawEvent->raw_values;
                Vector2i rawInput;

                for (int i = 0; i < rawEvent->valuators.mask_len * 8; i++)
                {
                    if (XIMaskIsSet(rawEvent->valuators.mask, i))
                    {
                        switch (i)
                        {
                            case 0:
                                rawInput.X() = *rawValue;
                                break;
                            case 1:
                                rawInput.Y() = *rawValue;
                                break;
                            default:
                                break;
                        }

                        rawValue++;
                    }
                }

                input->GetMouse()->AddRawMoveEvent(rawInput);
                break;
            }
            default:
                break;
        }
    }

    void X11WindowSystem::ProcessInputEvent(X11::XEvent& event)
    {
        InputService* input = _engine->GetService<InputService>();
        if (!input)
            return;

        switch (event.type)
        {
            case ButtonPress: // A mouse button was pressed
            {
                uint32 button = event.xbutton.button;

                switch (button)
                {
                    case 1: // Left
                    case 2: // Middle
                    case 3: // Right
                    {
                        input->GetMouse()->AddButtonStateChangeEvent(static_cast<MouseButton>(button - 1), true);
                        break;
                    }
                    case 4: // Scroll up
                        input->GetMouse()->AddScrollEvent(Vector2i::Up);
                        break;
                    case 5: // Scroll down
                        input->GetMouse()->AddScrollEvent(Vector2i::Down);
                        break;
                    case 6: // Scroll left
                        input->GetMouse()->AddScrollEvent(Vector2i::Left);
                        break;
                    case 7: // Scroll right
                        input->GetMouse()->AddScrollEvent(Vector2i::Right);
                        break;
                    case 8: // Extra button 3
                        input->GetMouse()->AddButtonStateChangeEvent(MouseButton::Button3, true);
                        break;
                    case 9: // Extra button 4
                        input->GetMouse()->AddButtonStateChangeEvent(MouseButton::Button4, true);
                        break;
                    default:
                        COCO_ENGINE_LOG_VERBOSE("Unknown mouse button pressed: %u", button);
                        break;
                }
                break;
            }
            case ButtonRelease: // A mouse button was released
            {
                uint32 button = event.xbutton.button;

                switch (button)
                {
                    case 1: // Left
                    case 2: // Middle
                    case 3: // Right
                    {
                        input->GetMouse()->AddButtonStateChangeEvent(static_cast<MouseButton>(button - 1), false);
                        break;
                    }
                    case 4: // Scroll up
                    case 5: // Scroll down
                    case 6: // Scroll left
                    case 7: // Scroll right
                        break;
                    case 8: // Extra button 3
                        input->GetMouse()->AddButtonStateChangeEvent(MouseButton::Button3, false);
                        break;
                    case 9: // Extra button 4
                        input->GetMouse()->AddButtonStateChangeEvent(MouseButton::Button4, false);
                        break;
                    default:
                        COCO_ENGINE_LOG_VERBOSE("Unknown mouse button released: %u", button);
                        break;
                }
                break;
            }
            case MotionNotify: // Called when the mouse moves in the window area
            {
                Vector2i pos(event.xmotion.x_root, event.xmotion.y_root);
                input->GetMouse()->AddPositionChangedEvent(pos);
                break;
            }
            case KeyPress: // Called when a key is pressed
            {
                uint32 keySym = X11::XLookupKeysym(&event.xkey, 0);
                KeyboardKey key = ToKeyboardKey(keySym);
                input->GetKeyboard()->AddKeyStateChangeEvent(key, true);
                break;
            }
            case KeyRelease: // Called when a key is released
            {
                uint32 keySym = X11::XLookupKeysym(&event.xkey, 0);
                KeyboardKey key = ToKeyboardKey(keySym);
                input->GetKeyboard()->AddKeyStateChangeEvent(key, false);
                break;
            }
            default:
                break;
        }
#endif
    }
} // Coco