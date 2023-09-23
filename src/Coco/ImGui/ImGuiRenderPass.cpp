#include "ImGuiRenderPass.h"

#include "ImGuiService.h"
#include <Coco/Core/Engine.h>
#include <imgui.h>

namespace Coco::ImGuiCoco
{
    const string ImGuiRenderPass::sPassName = "imgui";

    ImGuiRenderPass::ImGuiRenderPass() :
        _attachments({ AttachmentFormat(ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, false) })
    {}

    void ImGuiRenderPass::Execute(RenderContext& context, const RenderView& renderView)
    {
        ImDrawData* drawData = ::ImGui::GetDrawData();

        if (drawData->TotalVtxCount == 0)
            return;

        const ImGuiService* service = ImGuiService::cGet();
        if (!service)
            return;

        const ImGuiCocoPlatform* platform = service->GetPlatform();

        // Setup the state
        const MaterialData& material = renderView.GetMaterialData(platform->GetMaterial()->GetMaterialID());
        context.SetMaterial(material);
        context.SetMatrix4x4(UniformScope::Global, ShaderUniformData::MakeKey("projection"), renderView.GetProjectionMatrix());

        const MeshData& mesh = renderView.GetMeshData(platform->GetMesh()->GetID());

        Vector2Int clipOffset(static_cast<int>(drawData->DisplayPos.x), static_cast<int>(drawData->DisplayPos.y));
        uint32 indexOffset = 0;

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* drawList = drawData->CmdLists[n];

            for (int cmdI = 0; cmdI < drawList->CmdBuffer.Size; cmdI++)
            {
                const ImDrawCmd* cmd = &drawList->CmdBuffer[cmdI];

                Vector2Int clipMin(static_cast<int>(cmd->ClipRect.x) - clipOffset.X, static_cast<int>(cmd->ClipRect.y) - clipOffset.Y);
                Vector2Int clipMax(static_cast<int>(cmd->ClipRect.z) - clipOffset.X, static_cast<int>(cmd->ClipRect.w) - clipOffset.Y);

                context.SetScissorRect(RectInt(clipMin, clipMax));
                context.DrawIndexed(mesh, cmd->IdxOffset + indexOffset, cmd->ElemCount);
            }

            indexOffset += drawList->IdxBuffer.Size;
        }
    }
}