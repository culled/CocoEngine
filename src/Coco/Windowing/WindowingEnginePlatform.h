//
// Created by cullen on 2/27/26.
//

#ifndef COCOENGINE_WINDOWINGENGINEPLATFORM_H
#define COCOENGINE_WINDOWINGENGINEPLATFORM_H
#include <Coco/Core/Types/CoreTypes.h>
#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Memory/Refs.h>
#include "DisplayInfo.h"
#include "Window.h"

namespace Coco
{
    class GraphicsPlatform;

    class WindowingEnginePlatform
    {
    public:
        virtual ~WindowingEnginePlatform() = default;

        virtual uint16 GetDisplayCount() const = 0;
        virtual void GetDisplays(Span<DisplayInfo> outDisplays) = 0;
        virtual Ref<Window> CreatePlatformWindow(const WindowCreateParams& createParams) = 0;
        virtual uint64 GetWindowCount(bool includeInactive) const = 0;
        virtual void GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) = 0;
        virtual void DestroyWindows() = 0;
    };
} // Coco

#endif //COCOENGINE_WINDOWINGENGINEPLATFORM_H