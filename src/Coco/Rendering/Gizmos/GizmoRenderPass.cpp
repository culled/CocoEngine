#include "Renderpch.h"
#include "GizmoRenderPass.h"
#include "GizmoRender.h"

namespace Coco::Rendering
{
    const char* GizmoRenderPass::sPassName = "gizmo";

    const std::vector<AttachmentFormat> GizmoRenderPass::_sAttachments = {
        AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::Clear, false),
        AttachmentFormat(ImagePixelFormat::Depth32_Stencil8, ImageColorSpace::Linear, AttachmentClearMode::Clear, false)
    };

    void GizmoRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
    {
        context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("Projection"), renderView.GetProjectionMatrix());
        context.SetValue(UniformScope::Global, ShaderUniformData::MakeKey("View"), renderView.GetViewMatrix());
    }

    void GizmoRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
        renderView.FilterWithAllVisibilityGroups(objectIndices, GizmoRender::_sVisibilityGroup);

        context.SetShader(GizmoRender::_sShaderName);
        
        for (uint64 i : objectIndices)
        {
            const ObjectData& obj = renderView.GetRenderObject(i);
            const MeshData& mesh = renderView.GetMeshData(obj.MeshID);
        
            context.SetValue(UniformScope::Draw, ShaderUniformData::MakeKey("Model"), obj.ModelMatrix);
        
            if (const Color* color = std::any_cast<Color>(&obj.ExtraData))
            {
                context.SetValue(UniformScope::Draw, ShaderUniformData::MakeKey("Color"), *color);
            }
            else
            {
                context.SetValue(UniformScope::Draw, ShaderUniformData::MakeKey("Color"), Color::Magenta);
            }
        
            context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
        }
    }
}