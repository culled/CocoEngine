#include "RenderPass2D.h"

#include <Coco/Rendering/Resources/BuiltInShaders.h>

RenderPass2D::RenderPass2D() :
    _attachments({
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::Never, true)
        })
{
}

void RenderPass2D::Prepare(RenderContext& context, const RenderView& renderView)
{
    context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
    context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ViewMatrix"), renderView.GetViewMatrix());
}

void RenderPass2D::Execute(RenderContext& context, const RenderView& renderView)
{
    std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
    renderView.FilterWithShaderVariant(objectIndices, BuiltInShaders::UnlitVariant.Name);

    for (const uint64& i : objectIndices)
    {
        const ObjectData& obj = renderView.GetRenderObject(i);

        const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);

        context.SetMaterial(material, BuiltInShaders::UnlitVariant.Name);

        context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);

        context.DrawIndexed(renderView.GetMeshData(obj.MeshID), obj.IndexOffset, obj.IndexCount);
    }
}
