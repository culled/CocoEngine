//
// Created by cullen on 5/9/26.
//

#include "GizmosRenderPass.h"
#include "Coco/Rendering/RenderGraph/RenderGraphBuilder.h"
#include "Coco/Rendering/Graphics/Resources/RenderContext.h"
#include "Coco/Rendering/RenderScene.h"
#include "Coco/Rendering/Shader.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    uint64 GizmosRenderPass::_shaderResourceID = Resource::InvalidID;

    GizmosRenderPass::GizmosRenderPass()
    {
        EnsureDrawShader();
    }

    void GizmosRenderPass::Setup(RenderGraphBuilder& builder)
    {
        builder.WriteRenderTarget(0);
    }

    void GizmosRenderPass::Render(const RenderScene& sceneData, RenderContext& ctx) const
    {
        GraphicsPipelineState pipelineState;
        pipelineState.FillMode = PolygonFillMode::Line;
        pipelineState.TopologyMode = MeshTopologyMode::Lines;
        pipelineState.CullingMode = CullMode::None;
        pipelineState.BlendState = AttachmentBlendState::AlphaBlending;
        pipelineState.EnableDepthWrite = false;

        SharedPtr<Shader> shader = Engine::Get()->GetResourceManager()->GetResourceAs<Shader>(_shaderResourceID);
        ctx.SetShader(*shader, pipelineState);

        ShaderCursor cursor;
        if (ctx.CreateAndBindGlobalBuffer("cameraData", cursor))
        {
            cursor.Field("Projection").Write(sceneData.GetProjectionMatrix());
            cursor.Field("View").Write(sceneData.GetViewMatrix());
        }

        for (auto renderObject : sceneData.GetRenderObjectView())
        {
            if (!sceneData.HasData<GizmoObjectData>(renderObject.ID, true))
                continue;

            auto objData = sceneData.GetData<GizmoObjectData>(renderObject.ID, true);
            ctx.SetDrawData(objData, sizeof(GizmoObjectData), Span<const SharedPtr<Texture>>());

            ctx.DrawObject(renderObject);
        }
    }

    void GizmosRenderPass::EnsureDrawShader()
    {
        if (_shaderResourceID != Resource::InvalidID)
            return;

        auto shader = Engine::Get()->GetResourceManager()->CreateResource<Shader>("Gizmos Shader", "Shaders/BuiltIn/Gizmos.slang");
        _shaderResourceID = shader->GetID();
    }
} // Coco