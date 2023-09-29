#include "RenderPass2D.h"

RenderPass2D::RenderPass2D() :
    _attachments({
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, false)
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
    for (const ObjectData& obj : renderView.GetRenderObjects())
    {
        const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);
        const ShaderData& shader = renderView.GetShaderData(material.ShaderID);

        auto it = shader.RenderPassShaders.find(GetName());
        if (it == shader.RenderPassShaders.end())
            continue;

        context.SetShader(renderView.GetRenderPassShaderData(it->second));
        context.SetMaterial(material);

        context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);
        context.DrawIndexed(renderView.GetMeshData(obj.MeshID), obj.IndexOffset, obj.IndexCount);
    }
}
