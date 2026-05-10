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

    /// @brief Base class for a Linux-based windowing system
    class LinuxWindowSystem
    {
    public:
        virtual ~LinuxWindowSystem() = default;

        /// @brief Gets the number of connected displays
        /// @return The number of displays
        virtual uint16 GetDisplayCount() const = 0;

        /// @brief Gets information for the connected displays
        /// @param outDisplays Will be filled with the displays. Must be equal to the display count from GetDisplayCount()
        virtual void GetDisplays(Span<DisplayInfo> outDisplays) = 0;

        /// @brief Creates a new window
        /// @param createParams The window creation parameters
        /// @return The created window
        virtual Ref<Window> CreateWindow(const WindowCreateParams& createParams) = 0;

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

        /// @brief Called by the EnginePlatform to process pending operating-system events
        virtual void ProcessEvents() = 0;

        /// @brief Gets rendering extensions for the platform
        /// @param graphicsPlatform The graphics platform
        /// @param extensions The platform rendering extensions
        virtual void GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions) = 0;

        #ifdef COCO_SERVICE_INPUT
        /// @brief Determines if the platform supports mouse input
        /// @return True if the platform supports mouse input
        virtual bool SupportsMouse() = 0;

        /// @brief Determines if the platform supports keyboard input
        /// @return True if the platform supports keyboard input
        virtual bool SupportsKeyboard() = 0;

        /// @brief Determines if the platform supports raw input
        /// @return True if the platform supports raw input
        virtual bool SupportsRawInput() = 0;
        #endif

        /// @brief Gets the engine
        /// @return The engine
        Engine* GetEngine() { return _engine; }

    protected:
        Engine* _engine;

        LinuxWindowSystem(Engine* engine);
    };
} // Coco

#endif //COCOENGINE_LINUXWINDOWSYSTEM_H