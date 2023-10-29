#include "ImGuiRenderPass.h"

#include "ImGuiService.h"
#include <Coco/Rendering/Texture.h>
#include <Coco/Core/Engine.h>
#include <imgui.h>

namespace Coco::ImGuiCoco
{
    const string ImGuiRenderPass::sPassName = "imgui";

    ImGuiRenderPass::ImGuiRenderPass() :
        _attachments({ AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, AttachmentClearMode::Never, true) })
    {}

    void ImGuiRenderPass::Prepare(RenderContext& context, const RenderView& renderView)
    {
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("ProjectionMatrix"), renderView.GetProjectionMatrix());
    }

    void ImGuiRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        std::vector<uint64> objectIndices = renderView.GetRenderObjectIndices();
        renderView.FilterWithShaderVariant(objectIndices, ImGuiRenderPass::sPassName);

        for (uint64 i : objectIndices)
        {
            const ObjectData& obj = renderView.GetRenderObject(i);
            const ShaderData& shader = renderView.GetShaderData(obj.ShaderID);
            context.SetShader(shader, ImGuiRenderPass::sPassName);
            
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