//
// Created by cullen on 2/26/26.
//

#include "WindowService.h"
#include <Coco/Core/Engine.h>

#include "Coco/Core/Application.h"

namespace Coco
{
    WindowService::WindowService(Engine* engine) :
        EngineService(engine),
        _windowingPlatform(dynamic_cast<WindowingEnginePlatform*>(engine->GetPlatform())),
        _displays(),
        _mainWindow()
    {
        if (!_windowingPlatform)
            throw Exception("Platform does not support windowing");

        DisplaysChanged();

        COCO_ENGINE_LOG_VERBOSE("Created WindowService");
    }

    WindowService::~WindowService()
    {
        _mainWindow.Invalidate();
        _displays.Clear(true);

        _windowingPlatform->DestroyWindows();

        COCO_ENGINE_LOG_VERBOSE("Destroyed WindowService");
    }

    void WindowService::DisplaysChanged()
    {
        _displays.Clear();
        _displays.Resize(_windowingPlatform->GetDisplayCount());
        _windowingPlatform->GetDisplays(_displays);

        COCO_ENGINE_LOG_VERBOSE("Updated info for %u display(s)", _displays.GetCount());
    }

    Ref<Window> WindowService::CreateWindow(const WindowCreateParams& createParams)
    {
        Ref<Window> window = _windowingPlatform->CreatePlatformWindow(createParams);

        if (!_mainWindow)
            _mainWindow = window;

        return window;
    }

    uint64 WindowService::GetWindowCount(bool includeInactive) const
    {
        return _windowingPlatform->GetWindowCount(includeInactive);
    }

    void WindowService::GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) const
    {
        _windowingPlatform->GetWindows(includeInactive, outWindows);
    }
} // Coco