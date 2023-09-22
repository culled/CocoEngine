#pragma once
#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "ImGuiCocoPlatform.h"
#include "ImGuiRenderPass.h"
#include <Coco/Rendering/Pipeline/RenderPipeline.h>

namespace Coco::ImGui
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

    private:
        TickListener _newFrameTickListener;
        TickListener _drawTickListener;
        UniqueRef<ImGuiCocoPlatform> _platform;
        SharedRef<ImGuiRenderPass> _renderPass;
        UniqueRef<Rendering::RenderPipeline> _renderPipeline;

    public:
        ImGuiService();
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