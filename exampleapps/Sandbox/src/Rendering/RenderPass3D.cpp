#include "RenderPass3D.h"

#include <Coco/Rendering/Graphics/BufferDataWriter.h>
#include <Coco/Rendering/Resources/BuiltInShaders.h>
#include <Coco/Rendering/RenderService.h>

const BufferDataLayout RenderPass3D::sLightingBufferLayout = BufferDataLayout({
    BufferDataElement({ BufferDataType::Int }, 1),  // Directional light count
    BufferDataElement({                             // Directional lights
        BufferDataType::Float3, // Light direction
        BufferDataType::Float4  // Light color
        }, 2),
    BufferDataElement({BufferDataType::Int}, 1),    // Point light count
    BufferDataElement({                             // Point lights
        BufferDataType::Float3, // Light position
        BufferDataType::Float4  // Light color
        }, 10)
    });

RenderPass3D::RenderPass3D() :
    _attachments({
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::Clear, true),
        AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentClearMode::Clear, true),
        })
{}

void RenderPass3D::Prepare(RenderContext& context, const RenderView& renderView)
{
    context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
    context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ViewMatrix"), renderView.GetViewMatrix());
    context.SetFloat3(UniformScope::Global, ShaderUniformData::MakeKey("ViewPosition"), renderView.GetViewPosition());
    context.SetFloat4(UniformScope::Global, ShaderUniformData::MakeKey("AmbientColor"), Vector4(0.1, 0.1, 0.12, 1.0));

    BufferDataWriter writer(sLightingBufferLayout, RenderService::cGet()->GetDevice());

    {
        std::span<const DirectionalLightData> directionalLights = renderView.GetDirectionalLights();
        writer.WriteElement(0, 0, static_cast<int32>(directionalLights.size()));

        uint64 i = 0;
        for (const DirectionalLightData& d : directionalLights)
        {
            writer.WriteSubElement(1, i, 0, ShaderUniformData::ToFloat3(d.Direction));
            writer.WriteSubElement(1, i, 1, ShaderUniformData::ToFloat4(Vector4(d.Color.R, d.Color.G, d.Color.B, d.Color.A)));

            i++;
        }
    }

    {
        std::span<const PointLightData> pointLights = renderView.GetPointLights();
        writer.WriteElement(2, 0, static_cast<int32>(pointLights.size()));

        uint64 i = 0;
        for (const PointLightData& d : pointLights)
        {
            writer.WriteSubElement(3, i, 0, ShaderUniformData::ToFloat3(d.Position));
            writer.WriteSubElement(3, i, 1, ShaderUniformData::ToFloat4(Vector4(d.Color.R, d.Color.G, d.Color.B, d.Color.A)));

            i++;
        }
    }

    context.SetGlobalBufferData(ShaderUniformData::MakeKey("Lights"), 0, writer.GetRawData(), writer.GetSize());
}

void RenderPass3D::Execute(RenderContext& context, const RenderView& renderView)
{
    std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
    renderView.FilterWithShaderVariant(objectIndices, BuiltInShaders::LitVariant.Name);
    renderView.FilterOutsideFrustum(objectIndices);

    for (const uint64& i : objectIndices)
    {
        const ObjectData& obj = renderView.GetRenderObject(i);

        const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);

        context.SetMaterial(material, BuiltInShaders::LitVariant.Name);

        context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("ModelMatrix"), obj.ModelMatrix);

        context.DrawIndexed(renderView.GetMeshData(obj.MeshID), obj.IndexOffset, obj.IndexCount);
    }
}
