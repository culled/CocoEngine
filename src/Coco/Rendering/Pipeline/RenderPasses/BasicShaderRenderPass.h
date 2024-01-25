#pragma once
#include "../../Renderpch.h"
#include "../RenderPass.h"

namespace Coco::Rendering
{
    class BasicShaderRenderPass :
        public RenderPass
    {
    public:
        BasicShaderRenderPass(
            SharedRef<Shader> shader,
            std::span<const RenderPassAttachment> attachmentFormats,
            uint64 visibilityGroups = 1,
            bool exclusiveVisibilityGroups = false,
            bool useFrustumCulling = false);

        // Inherited via RenderPass
        const string& GetName() const override { return _passName; }
        std::span<const RenderPassAttachment> GetAttachments() const override { return _attachments; }
        void Execute(RenderContext& context, const RenderView& renderView) override;

    private:
        string _passName;
        SharedRef<Shader> _shader;
        std::vector<RenderPassAttachment> _attachments;
        uint64 _visibilityGroups;
        bool _exclusiveVisibilityGroups;
        bool _useFrustumCulling;
    };
}