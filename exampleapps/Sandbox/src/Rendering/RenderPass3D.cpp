#include "RenderPass3D.h"

#include <Coco/Rendering/Graphics/BufferDataWriter.h>
#include <Coco/Rendering/RenderService.h>

const BufferDataLayout RenderPass3D::sLightingBufferLayout = BufferDataLayout({
    BufferDataElement({ BufferDataType::Int }, 1),
    BufferDataElement({
        BufferDataType::Float3,
        BufferDataType::Float4
        }, 2)
    });

RenderPass3D::RenderPass3D() :
    _attachments({
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, true),
        AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, true),
        })
{}

void RenderPass3D::Prepare(RenderContext& context, const RenderView& renderView)
{
    context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
    context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ViewMatrix"), renderView.GetViewMatrix());
    context.SetFloat3(UniformScope::Global, ShaderUniformData::MakeKey("ViewPosition"), renderView.GetViewPosition());
    context.SetFloat4(UniformScope::Global, ShaderUniformData::MakeKey("AmbientColor"), Vector4(0.1, 0.1, 0.12, 1.0));

    {
        BufferDataWriter writer(sLightingBufferLayout, RenderService::cGet()->GetDevice());

        std::span<const DirectionalLightData> directionalLights = renderView.GetDirectionalLights();
        writer.WriteElement(0, 0, static_cast<int32>(directionalLights.size()));

        uint64 i = 0;
        for (const DirectionalLightData& d : renderView.GetDirectionalLights())
        {
            writer.WriteSubElement(1, i, 0, ShaderUniformData::ToFloat3(d.Direction));
            writer.WriteSubElement(1, i, 1, ShaderUniformData::ToFloat4(Vector4(d.Color.R, d.Color.G, d.Color.B, d.Color.A)));

            i++;
        }

        context.SetGlobalBufferData(ShaderUniformData::MakeKey("DirectionalLights"), 0, writer.GetRawData(), writer.GetSize());
    }
}

void RenderPass3D::Execute(RenderContext& context, const RenderView& renderView)
{
    for (const ObjectData& obj : renderView.GetRenderObjects())
    {
        if (!renderView.GetViewFrustum().IsInside(obj.Bounds))
            continue;

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
