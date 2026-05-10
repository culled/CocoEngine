//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_X11WINDOWSYSTEM_H
#define COCOENGINE_X11WINDOWSYSTEM_H
#include "../LinuxWindowSystem.h"

#include "Coco/Core/Types/Map.h"
#include "X11Types.h"

namespace Coco
{
    class X11Window;
    class GraphicsSurface;
    class GraphicsPlatform;

    /// @brief An implementation of a LinuxWindowSystem for X11
    class X11WindowSystem : public LinuxWindowSystem
    {
        friend class X11Window;

    public:
        /// @brief The atom for the WM_DELETE_WINDOW message
        static X11::Atom WM_DELETE_WINDOW;

        /// @brief The atom for the WM_STATE message
        static X11::Atom WM_STATE;

        /// @brief The atom for the NET_WM_STATE message
        static X11::Atom NET_WM_STATE;

        /// @brief The atom for the NET_WM_STATE_MAXIMIZED_VERT message
        static X11::Atom NET_WM_STATE_MAXIMIZED_VERT;

        /// @brief The atom for the NET_WM_STATE_MAXIMIZED_HORZ message
        static X11::Atom NET_WM_STATE_MAXIMIZED_HORZ;

        /// @brief The atom for the NET_WM_STATE_HIDDEN message
        static X11::Atom NET_WM_STATE_HIDDEN;

        /// @brief The atom for the NET_WM_STATE_FULLSCREEN message
        static X11::Atom NET_WM_STATE_FULLSCREEN;

        X11WindowSystem(Engine* engine);
        ~X11WindowSystem();

        void GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions) override;
        uint16 GetDisplayCount() const override;
        void GetDisplays(Span<DisplayInfo> outDisplays) override;
        Ref<Window> CreateWindow(const WindowCreateParams& createParams) override;
        uint64 GetWindowCount(bool includeInactive) const override;
        void GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) override;
        void DestroyWindows() override;
        void ProcessEvents() override;

        /// @brief Gets the main X11 display (not to be confused with a display monitor)
        /// @return The main X11 display
        X11::Display* GetMainDisplay() { return _mainDisplay; }

        /// @brief Gets the root X11 window
        /// @return The root X11 window
        X11::Window GetRootWindow() const { return _rootWindow; }

        /// @brief Creates a graphics surface for a window that can be used to draw to it
        /// @param graphicsPlatform The graphics platform
        /// @param window The window
        /// @return A graphics surface for the window
        Ref<GraphicsSurface> CreateWindowSurface(GraphicsPlatform& graphicsPlatform, X11Window& window);

    private:
        X11::Display* _mainDisplay;
        X11::Window _rootWindow;
        uint32 _nextWindowID;
        Map<uint32, ManagedRef<X11Window>> _activeWindows;
        Map<X11::Window, Ref<X11Window>> _x11WindowToActiveWindow;

        /// @brief Processes the given event from the X11 server
        /// @param event The event
        void ProcessEvent(X11::XEvent& event);

        /// @brief Dispatches a window-specific event to be processed by its window
        /// @param event The event
        void DispatchWindowEvent(const X11::XEvent& event);

        #ifdef COCO_SERVICE_INPUT
    public:
        bool SupportsMouse() override { return true; }
        bool SupportsKeyboard() override { return true; }
        bool SupportsRawInput() override { return _rawInputAvailable; }

    private:
        bool _rawInputAvailable;
        int _rawInputOpcode;

        /// @brief Sets up raw input if the platform supports it
        void SetupRawInput();

        /// @brief Processes a raw input event from the X11 server
        /// @param event The raw input event
        void ProcessRawInputEvent(X11::XEvent& event);

        /// @brief Processes an input event from the X11 server
        /// @param event The input event
        void ProcessInputEvent(X11::XEvent& event);
        #endif
    };
} // Coco

#endif //COCOENGINE_X11WINDOWSYSTEM_H