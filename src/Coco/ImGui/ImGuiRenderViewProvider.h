#pragma once
#include <Coco\Rendering\Providers\RenderViewDataProvider.h>
#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>

struct ImGuiViewport;

namespace Coco::ImGuiCoco
{
    class ImGuiRenderViewProvider :
        public Rendering::RenderViewDataProvider
    {
    public:
        static const Rendering::ShaderUniformLayout GlobalUniformLayout;

    public:
        ImGuiRenderViewProvider(ImGuiViewport* viewport);

        void SetupRenderView(
            Rendering::RenderView& renderView,
            const Rendering::CompiledRenderPipeline& pipeline,
            uint64 rendererID,
            const SizeInt& backbufferSize,
            std::span<Ref<Rendering::Image>> backbuffers) final;

    private:
        ImGuiViewport* _viewport;
    };
}