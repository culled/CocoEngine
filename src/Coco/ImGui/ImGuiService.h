#pragma once
#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "ImGuiCocoPlatform.h"
#include <Coco/Core/Events/Query.h>

namespace Coco::ImGuiCoco
{
    /// @brief An EngineService that provides ImGui functionality
    class ImGuiService :
        public EngineService,
        public Singleton<ImGuiService>
    {
    public:
        /// @brief The priority of the tick that starts the ImGui frame
        static const int sImGuiNewFramePriority;

        /// @brief The priority of the tick that draws the ImGui frame
        static const int sImGuiDrawPriority;

        /// @brief A handler for a query that determines if ImGui wants to capture the mouse (i.e. mouse events should be ignored)
        QueryHandler<bool> HasMouseCapture;

        /// @brief A handler for a query that determines if ImGui wants to capture the keyboard (i.e. keyboard events should be ignored)
        QueryHandler<bool> HasKeyboardCapture;

    private:
        ManagedRef<TickListener> _newFrameTickListener;
        ManagedRef<TickListener> _drawTickListener;
        UniqueRef<ImGuiCocoPlatform> _platform;

    public:
        ImGuiService(bool enableViewports);
        ~ImGuiService();

        /// @brief Gets the ImGui platform
        /// @return The platform
        ImGuiCocoPlatform* GetPlatform() { return _platform.get(); }

        /// @brief Gets the ImGui platform
        /// @return The platform
        const ImGuiCocoPlatform* GetPlatform() const { return _platform.get(); }

    private:
        /// @brief Handles the new frame tick
        /// @param tickInfo The tick info
        void HandleNewFrameTick(const TickInfo& tickInfo);

        /// @brief Handles the draw tick
        /// @param tickInfo The tick info
        void HandleDrawTick(const TickInfo& tickInfo);
    };
}