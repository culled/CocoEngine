#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>

using namespace Coco::Rendering;

namespace Coco
{
    class TestRenderPass :
        public RenderPass
    {
    private:
        static std::vector<AttachmentFormat> _sAttachments;

    public:
        // Inherited via RenderPass
        const char* GetName() const override { return "test"; }
        std::span<const AttachmentFormat> GetInputAttachments() const override { return _sAttachments; }
        bool SupportsMSAA() const override { return true; }

        void Prepare(RenderContext& context, const RenderView& renderView) override;
        void Execute(RenderContext& context, const RenderView& renderView) override;
    };
}