#include "RenderPass2D.h"

#include <Coco/Rendering/Resources/BuiltInShaders.h>
#include <Coco/Core/Engine.h>

SharedRef<Shader> RenderPass2D::_sShader = nullptr;

RenderPass2D::RenderPass2D() :
    _attachments({
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, true)
        })
{
    _sShader = Engine::Get()->GetResourceLibrary().Create<Shader>("UI", BuiltInShaders::GetUnlitShader());

    std::array<BlendState, 1> blendStates = { BlendState::AlphaBlending };
    _sShader->SetAttachmentBlendStates(blendStates);
}

void RenderPass2D::Prepare(RenderContext& context, const RenderView& renderView)
{
    context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
    context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ViewMatrix"), renderView.GetViewMatrix());
}

void RenderPass2D::Execute(RenderContext& context, const RenderView& renderView)
{
    std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
    renderView.FilterWithAnyVisibilityGroups(objectIndices, 1);

    context.SetShader(_sShader->GetName());

    for (const uint64& i : objectIndices)
    {
        const ObjectData& obj = renderView.GetRenderObject(i);

        if (obj.MaterialID != RenderView::InvalidID)
        {
            const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);
            context.SetMaterial(material);
        }

        context.SetValue(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);

        context.DrawIndexed(renderView.GetMeshData(obj.MeshID), obj.IndexOffset, obj.IndexCount);
    }
}
