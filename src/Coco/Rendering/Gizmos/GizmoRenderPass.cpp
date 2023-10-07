#include "Renderpch.h"
#include "GizmoRenderPass.h"

namespace Coco::Rendering
{
    const char* GizmoRenderPass::sPassName = "gizmo";

    const std::vector<AttachmentFormat> GizmoRenderPass::_sAttachments = {
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::ClearOnFirstUse),
        AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentClearMode::ClearOnFirstUse)
    };

    void GizmoRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
    {
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("Projection"), renderView.GetProjectionMatrix());
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("View"), renderView.GetViewMatrix());
    }

    void GizmoRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
        renderView.FilterWithShaderVariant(objectIndices, sPassName);

        for (uint64 i : objectIndices)
        {
            const ObjectData& obj = renderView.GetRenderObject(i);
            const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);
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