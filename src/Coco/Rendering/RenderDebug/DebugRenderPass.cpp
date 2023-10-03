#include "Renderpch.h"
#include "DebugRenderPass.h"

namespace Coco::Rendering
{
    const char* DebugRenderPass::sPassName = "debug";

    const std::vector<AttachmentFormat> DebugRenderPass::_sAttachments = {
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::ClearOnFirstUse),
        AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentClearMode::ClearOnFirstUse)
    };

    void DebugRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
    {
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("Projection"), renderView.GetProjectionMatrix());
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("View"), renderView.GetViewMatrix());
    }

    void DebugRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        for (const ObjectData& obj : renderView.GetRenderObjects())
        {
            const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);
            const ShaderData& shader = renderView.GetShaderData(material.ShaderID);

            if (shader.GroupTag != sPassName)
                continue;

            const MeshData& mesh = renderView.GetMeshData(obj.MeshID);

            context.SetMaterial(material, sPassName);

            context.SetMatrix4x4(UniformScope::Draw, ShaderUniformData::MakeKey("Model"), obj.ModelMatrix);

            if (const Color* color = std::any_cast<Color>(&obj.ExtraData))
            {
                context.SetFloat4(UniformScope::Draw, ShaderUniformData::MakeKey("Color"), *color);
            }
            else
            {
                context.SetFloat4(UniformScope::Draw, ShaderUniformData::MakeKey("Color"), Color::Magenta);
            }

            context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
        }
    }
}