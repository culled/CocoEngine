//
// Created by cullen on 4/6/26.
//

#include "ImGuiRenderPass.h"

#include "Coco/Rendering/Graphics/ShaderCursor.h"
#include "Coco/Rendering/Graphics/Resources/RenderContext.h"
#include "Coco/Rendering/RenderGraph/RenderGraphBuilder.h"
#include "Coco/Rendering/Shader.h"
#include "Coco/Rendering/RenderScene.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    uint64 ImGuiRenderPass::_shaderResourceID = Resource::InvalidID;

    ImGuiRenderPass::ImGuiRenderPass(const Matrix4x4& projection) :
        _projection(projection)
    {
        EnsureDrawShader();
    }

    void ImGuiRenderPass::Setup(RenderGraphBuilder& builder)
    {
        builder.WriteRenderTarget(0);
    }

    void ImGuiRenderPass::Render(const RenderScene& sceneData, RenderContext& ctx) const
    {
        GraphicsPipelineState pipelineState;
        pipelineState.CullingMode = CullMode::None;
        pipelineState.BlendState = AttachmentBlendState::AlphaBlending;
        pipelineState.EnableDepthWrite = false;
        pipelineState.DepthTestMode = DepthTestingMode::Never;

        SharedPtr<Shader> shader = Engine::Get()->GetResourceManager()->GetResourceAs<Shader>(_shaderResourceID);
        ctx.SetShader(*shader, pipelineState);

        ShaderCursor cursor;
        if (ctx.CreateAndBindGlobalBuffer("imguiCameraData", cursor))
        {
            cursor.Field("Projection").Write(_projection);
        }

        for (auto renderObject : sceneData.GetRenderObjectView())
        {
            if (!sceneData.HasData<ImGuiObjectData>(renderObject.ID, true))
                continue;

            auto objData = sceneData.GetData<ImGuiObjectData>(renderObject.ID, true);
            ctx.SetScissor(objData->ScissorRect);

            ctx.SetDrawData(nullptr, 0, Span<const SharedPtr<Texture>>({objData->DrawTexture}));

            ctx.DrawObject(renderObject);
        }
    }

    void ImGuiRenderPass::EnsureDrawShader()
    {
        if (_shaderResourceID != Resource::InvalidID)
            return;

        auto shader = Engine::Get()->GetResourceManager()->CreateResource<Shader>("ImGui Shader", "Shaders/BuiltIn/ImGui.slang");
        _shaderResourceID = shader->GetID();
    }
} // Coco