#include "ImGuiRenderPass.h"

#include "ImGuiService.h"
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/Engine.h>
#include <imgui.h>

namespace Coco::ImGuiCoco
{
    const string ImGuiRenderPass::sPassName = "imgui";

    ImGuiRenderPass::ImGuiRenderPass() :
        _attachments({ AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::Never) })
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
            context.SetMaterial(material, ImGuiRenderPass::sPassName);
            
            if (obj.ExtraData.has_value())
            {
                ShaderUniformData::TextureSampler sampler = std::any_cast<ShaderUniformData::TextureSampler>(obj.ExtraData);

                context.SetTextureSampler(UniformScope::Draw, ShaderUniformData::MakeKey("Texture"), sampler.first, sampler.second);
            }

            const MeshData& mesh = renderView.GetMeshData(obj.MeshID);
            context.SetScissorRect(obj.ScissorRect);
            context.DrawIndexed(mesh, obj.IndexOffset, obj.IndexCount);
        }
    }
}