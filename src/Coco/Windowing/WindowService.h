//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_WINDOWSERVICE_H
#define COCOENGINE_WINDOWSERVICE_H
#include "DisplayInfo.h"
#include "Window.h"
#include "WindowingEnginePlatform.h"
#include "Coco/Core/EngineService.h"
#include "Coco/Core/Types/Array.h"

namespace Coco
{
    class WindowService : public EngineService
    {
    public:
        WindowService(Engine* engine);
        ~WindowService();

        void DisplaysChanged();

        Ref<Window> CreateWindow(const WindowCreateParams& createParams);
        bool IsMainWindow(const Window& window) const { return _mainWindow.Get() == &window;}
        Ref<Window> GetMainWindow() { return _mainWindow; }
        uint64 GetWindowCount(bool includeInactive) const;
        void GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) const;

        Span<const DisplayInfo> GetDisplays() const { return _displays; }

    private:
        WindowingEnginePlatform* _windowingPlatform;
        Array<DisplayInfo> _displays;
        Ref<Window> _mainWindow;
    };
} // Coco

#endif //COCOENGINE_WINDOWSERVICE_H