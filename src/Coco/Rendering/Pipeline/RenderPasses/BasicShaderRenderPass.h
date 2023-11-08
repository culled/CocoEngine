#pragma once
#include "../RenderPass.h"

#include "../../Shader.h"

namespace Coco::Rendering
{
    /// @brief A renderpass for a specific shader that only has instance uniforms and a model matrix property
    class BasicShaderRenderPass :
        public RenderPass
    {
    private:
        static const std::array<AttachmentFormat, 2> _sAttachments;

        string _passName;
        SharedRef<Shader> _shader;
        uint64 _visibilityGroups;
        bool _exclusiveVisibilityGroups;
        bool _useFrustumCulling;

    public:
        BasicShaderRenderPass(SharedRef<Shader> shader, uint64 visibilityGroups = 1, bool exclusiveVisibilityGroups = false, bool useFrustumCulling = true);

        // Inherited via RenderPass
        const char* GetName() const final { return _passName.c_str(); }
        std::span<const AttachmentFormat> GetInputAttachments() const final { return _sAttachments; }
        void Prepare(RenderContext& context, const RenderView& renderView) final;
        void Execute(RenderContext& context, const RenderView& renderView) final;
    };
}