#pragma once
#include <Coco/Rendering/Pipeline/RenderPass.h>

#include <vector>

namespace Coco::ImGui
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
        string GetName() const final { return sPassName; }
        std::span<const AttachmentFormat> GetInputAttachments() const final { return _attachments; }
        void Execute(RenderContext& context, const RenderView& renderView) override;
    };
}