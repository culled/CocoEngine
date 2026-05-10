//
// Created by cullen on 2/27/26.
//

#ifndef COCOENGINE_WINDOWINGENGINEPLATFORM_H
#define COCOENGINE_WINDOWINGENGINEPLATFORM_H
#include <Coco/Core/Types/CoreTypes.h>
#include <Coco/Core/Memory/Refs.h>
#include "DisplayInfo.h"
#include "Window.h"

namespace Coco
{
    class GraphicsPlatform;

    /// @brief A class that an EnginePlatform that supports GUI windows should implement
    class WindowingEnginePlatform
    {
    public:
        virtual ~WindowingEnginePlatform() = default;

        /// @brief Gets the number of connected displays
        /// @return The number of displays
        virtual uint16 GetDisplayCount() const = 0;

        /// @brief Gets information for the connected displays
        /// @param outDisplays Will be filled with the displays. Must be equal to the display count from GetDisplayCount()
        virtual void GetDisplays(Span<DisplayInfo> outDisplays) = 0;

        /// @brief Creates a new window
        /// @param createParams The window creation parameters
        /// @return The created window
        virtual Ref<Window> CreatePlatformWindow(const WindowCreateParams& createParams) = 0;

        /// @brief Gets the number of visible windows
        /// @param includeInactive If true, also include the number of windows not currently visible
        /// @return The number of windows
        virtual uint64 GetWindowCount(bool includeInactive) const = 0;

        /// @brief Gets references to all visible windows
        /// @param includeInactive If true, also include windows not currently visible
        /// @param outWindows Will be filled with references to the windows. Must be as large as the value returned from GetWindowCount()
        virtual void GetWindows(bool includeInactive, Span<Ref<Window>> outWindows) = 0;

        /// @brief Destroys all windows
        virtual void DestroyWindows() = 0;
    };
} // Coco

#endif //COCOENGINE_WINDOWINGENGINEPLATFORM_H