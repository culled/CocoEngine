//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_LINUXWINDOWSYSTEM_H
#define COCOENGINE_LINUXWINDOWSYSTEM_H
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Windowing/DisplayInfo.h"
#include <Coco/Core/Memory/Refs.h>
#include <Coco/Windowing/Window.h>

#include "Coco/Rendering/RenderingEnginePlatform.h"

namespace Coco
{
    class Engine;

    class LinuxWindowSystem
    {
    public:
        virtual ~LinuxWindowSystem() = default;

        virtual uint16 GetDisplayCount() const = 0;
        virtual void GetDisplays(Span<DisplayInfo> outDisplays) = 0;
        virtual Ref<Window> CreateWindow(const WindowCreateParams& createParams) = 0;
        virtual uint64 GetWindowCount(bool includeInactive) const = 0;
        virtual void GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) = 0;
        virtual void DestroyWindows() = 0;
        virtual void ProcessEvents() = 0;
        virtual void GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions) = 0;

        #ifdef COCO_SERVICE_INPUT
        virtual bool SupportsMouse() = 0;
        virtual bool SupportsKeyboard() = 0;
        virtual bool SupportsRawInput() = 0;
        #endif

        Engine* GetEngine() { return _engine; }

    protected:
        Engine* _engine;

    protected:
        LinuxWindowSystem(Engine* engine);
    };
} // Coco

#endif //COCOENGINE_LINUXWINDOWSYSTEM_H