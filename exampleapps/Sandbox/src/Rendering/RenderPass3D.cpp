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

SharedRef<Shader> RenderPass3D::_sShader = nullptr;

RenderPass3D::RenderPass3D() :
    _attachments({
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB),
        AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear),
        })
{
    _sShader = BuiltInShaders::GetLitShader();
}

void RenderPass3D::Prepare(RenderContext& context, const RenderView& renderView)
{
    context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
    context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ViewMatrix"), renderView.GetViewMatrix());
    context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("ViewPosition"), renderView.GetViewPosition());
    context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("AmbientColor"), Vector4(0.1, 0.1, 0.12, 1.0));

    BufferDataWriter writer(sLightingBufferLayout, RenderService::cGet()->GetDevice());

    {
        std::span<const DirectionalLightData> directionalLights = renderView.GetDirectionalLights();
        writer.WriteElement(0, 0, static_cast<int32>(directionalLights.size()));

        uint64 i = 0;
        for (const DirectionalLightData& d : directionalLights)
        {
            writer.WriteSubElement(1, i, 0, d.Direction.AsFloatArray());
            writer.WriteSubElement(1, i, 1, d.Color.AsFloatArray(true));

            i++;
        }
    }

    {
        std::span<const PointLightData> pointLights = renderView.GetPointLights();
        writer.WriteElement(2, 0, static_cast<int32>(pointLights.size()));

        uint64 i = 0;
        for (const PointLightData& d : pointLights)
        {
            writer.WriteSubElement(3, i, 0, d.Position.AsFloatArray());
            writer.WriteSubElement(3, i, 1, d.Color.AsFloatArray(true));

            i++;
        }
    }

    context.SetGlobalBufferData(ShaderUniformData::MakeKey("Lights"), 0, writer.GetRawData(), writer.GetSize());
}

void RenderPass3D::Execute(RenderContext& context, const RenderView& renderView)
{
    std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
    renderView.FilterWithAnyVisibilityGroups(objectIndices, 1);
    renderView.FilterOutsideFrustum(objectIndices);

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
