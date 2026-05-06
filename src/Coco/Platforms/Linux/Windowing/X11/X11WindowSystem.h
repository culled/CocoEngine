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

    class X11WindowSystem : public LinuxWindowSystem
    {
        friend class X11Window;

    public:
        static X11::Atom WM_DELETE_WINDOW;
        static X11::Atom WM_STATE;
        static X11::Atom NET_WM_STATE;
        static X11::Atom NET_WM_STATE_MAXIMIZED_VERT;
        static X11::Atom NET_WM_STATE_MAXIMIZED_HORZ;
        static X11::Atom NET_WM_STATE_HIDDEN;
        static X11::Atom NET_WM_STATE_FULLSCREEN;

    public:
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

        X11::Display* GetMainDisplay() { return _mainDisplay; }
        X11::Window GetRootWindow() const { return _rootWindow; }
        Ref<GraphicsSurface> CreateWindowSurface(GraphicsPlatform& graphicsPlatform, X11Window& window);

    private:
        X11::Display* _mainDisplay;
        X11::Window _rootWindow;
        uint32 _nextWindowID;
        Map<uint32, ManagedRef<X11Window>> _activeWindows;
        Map<X11::Window, Ref<X11Window>> _x11WindowToActiveWindow;

    private:
        void ProcessEvent(X11::XEvent& event);
        void DispatchWindowEvent(const X11::XEvent& event);

        #ifdef COCO_SERVICE_INPUT
    public:
        bool SupportsMouse() override { return true; }
        bool SupportsKeyboard() override { return true; }
        bool SupportsRawInput() override { return _rawInputAvailable; }

    private:
        bool _rawInputAvailable;
        int _rawInputOpcode;

    private:
        void SetupRawInput();
        void ProcessRawInputEvent(X11::XEvent& event);
        void ProcessInputEvent(X11::XEvent& event);
        #endif
    };
} // Coco

#endif //COCOENGINE_X11WINDOWSYSTEM_H