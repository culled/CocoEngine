#pragma once
#include <Coco/Rendering/Pipeline/RenderPass.h>

#include <vector>

namespace Coco::ImGuiCoco
{
    using namespace Coco::Rendering;

    /// @brief A RenderPass for ImGui
    class ImGuiRenderPass :
        public RenderPass
    {
    public:
        static const string sPassName;

    private:
        std::vector<AttachmentFormat> _attachments;

    public:
        ImGuiRenderPass();

        // Inherited via RenderPass
        const char* GetName() const final { return sPassName.c_str(); }
        std::span<const AttachmentFormat> GetInputAttachments() const final { return _attachments; }
        bool SupportsMSAA() const final { return false; }
        void Prepare(RenderContext& context, const RenderView& renderView) override;
        void Execute(RenderContext& context, const RenderView& renderView) override;
    };
}