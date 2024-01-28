#pragma once
#include "../Pipeline/RenderPass.h"

namespace Coco::Rendering
{
    class Mesh;
    class Shader;

    class GizmoRenderPass :
        public RenderPass
    {
    public:
        GizmoRenderPass(bool clearGizmoDrawCalls = true);

        // Inherited via RenderPass
        const string& GetName() const override { return _name; }
        std::span<const RenderPassAttachment> GetAttachments() const override { return _attachments; }
        void Execute(RenderContext& context, const RenderView& renderView) override;

    private:
        static const string _name;
        static std::array<RenderPassAttachment, 2> _attachments;
        static SharedRef<Mesh> _gizmoMesh;
        static SharedRef<Shader> _gizmoShader;

        bool _clearDrawCalls;

    private:
        void EnsureGizmoResources();
        void CreateGizmoMesh();
        void CreateGizmoShader();
    };
}