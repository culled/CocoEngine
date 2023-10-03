#include "ImGuiRenderPass.h"

#include "ImGuiService.h"
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/Engine.h>
#include <imgui.h>

namespace Coco::ImGuiCoco
{
    const string ImGuiRenderPass::sPassName = "imgui";

    ImGuiRenderPass::ImGuiRenderPass() :
        _attachments({ AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, false) })
    {}

    void ImGuiRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
    {
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("Projection"), renderView.GetProjectionMatrix());
    }

    void ImGuiRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        for (const ObjectData& obj : renderView.GetRenderObjects())
        {
            const MaterialData& material = renderView.GetMaterialData(obj.MaterialID);
            context.SetMaterial(material);
            
            if (obj.ExtraData.has_value())
            {
                Texture* tex = std::any_cast<Texture*>(obj.ExtraData);

                Assert(tex != nullptr)

                context.SetTextureSampler(UniformScope::Draw, ShaderUniformData::MakeKey("Texture"), tex->GetImage(), tex->GetImageSampler());
            }

            const MeshData& mesh = renderView.GetMeshData(obj.MeshID);
            context.SetScissorRect(obj.ScissorRect);
            context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
        }
    }
}