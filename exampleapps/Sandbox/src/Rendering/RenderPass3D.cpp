#include "RenderPass3D.h"

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
        std::span<const DirectionalLightData> directionalLights = renderView.GetDirectionalLights();
        std::vector<uint8> directionalLightBufferData;
        directionalLightBufferData.resize(GetDataTypeSize(BufferDataType::Int));
        int32 size = static_cast<int32>(directionalLights.size());
        Assert(memcpy_s(directionalLightBufferData.data(), directionalLightBufferData.size(), &size, GetDataTypeSize(BufferDataType::Int)) == 0)

        uint64 offset = GetDataTypeSize(BufferDataType::Float4);
        for (const DirectionalLightData& d : directionalLights)
        {
            DirectionalLight l{};
            l.Direction = ShaderUniformData::ToFloat3(d.Direction);
            l.Color = ShaderUniformData::ToFloat4(Vector4(d.Color.R, d.Color.G, d.Color.B, d.Color.A));

            directionalLightBufferData.resize(offset + sizeof(DirectionalLight));
            Assert(memcpy_s(directionalLightBufferData.data() + offset, directionalLightBufferData.size(), &l, sizeof(DirectionalLight)) == 0)
            offset += sizeof(DirectionalLight);
        }

        context.SetGlobalBufferData(ShaderUniformData::MakeKey("DirectionalLights"), 0, directionalLightBufferData.data(), directionalLightBufferData.size());
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
