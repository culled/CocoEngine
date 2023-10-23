#pragma once
#include "../Pipeline/RenderPass.h"

namespace Coco::Rendering
{
    /// @brief A RenderPass that draws gizmos
    class GizmoRenderPass :
        public RenderPass
    {
    public:
        /// @brief The name of the debug render pass
        static const char* sPassName;

    private:
        static const std::vector<AttachmentFormat> _sAttachments;

    public:
        // Inherited via RenderPass
        const char* GetName() const final { return sPassName; }
        std::span<const AttachmentFormat> GetInputAttachments() const final { return _sAttachments; }
        bool SupportsMSAA() const { return true; }
        void Prepare(RenderContext& context, const RenderView& renderView) final;
        void Execute(RenderContext& context, const RenderView& renderView) final;
    };
}