#pragma once
#include <Coco\Rendering\Pipeline\RenderPass.h>
#include <Coco/Core/Types/Vector.h>

namespace Coco
{
    class PickingRenderPass :
        public Rendering::RenderPass
    {
    public:
        static const string sName;
        
    private:
        static std::vector<Rendering::AttachmentFormat> _sAttachments;

    public:
        // Inherited via RenderPass
        const char* GetName() const override { return sName.c_str(); }
        std::span<const Rendering::AttachmentFormat> GetInputAttachments() const override { return _sAttachments; }
        bool SupportsMSAA() const override { return true; }
        void Prepare(Rendering::RenderContext& context, const Rendering::RenderView& renderView) override;
        void Execute(Rendering::RenderContext& context, const Rendering::RenderView& renderView) override;
    };
}