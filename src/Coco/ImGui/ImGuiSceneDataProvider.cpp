#include "ImGuiSceneDataProvider.h"

#include <Coco/Rendering/Mesh.h>
#include <Coco/Rendering/Graphics/ShaderUniformLayout.h>
#include <Coco/Rendering/Texture.h>
#include <Coco/Rendering/RenderService.h>
#include <Coco/Core/Engine.h>

#include "ImGuiRenderPass.h"
#include <imgui.h>

using namespace Coco::Rendering;

namespace Coco::ImGuiCoco
{
    ImGuiExtraData::ImGuiExtraData(const RectInt& scissorRect, const ResourceID& overrideTextureID) :
        ScissorRect(scissorRect),
        OverrideTextureID(overrideTextureID)
    {}

	ImGuiSceneDataProvider::ImGuiSceneDataProvider(ImGuiViewport* viewport) :
		_viewport(viewport)
	{
		CocoAssert(_viewport, "Viewport was null")
	}

    void ImGuiSceneDataProvider::DestroyViewportMesh(ImGuiViewport* viewport)
    {
        ResourceID id = CreateResourceIDForViewport(viewport);

        ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

        if(resources.Has(id))
            resources.Remove(id);
    }

	void ImGuiSceneDataProvider::GatherSceneData(RenderView& renderView)
	{
        ImDrawData* drawData = _viewport->DrawData;

        VertexDataFormat format(VertexAttrFlags::Color | VertexAttrFlags::UV0);
        SharedRef<Mesh> mesh = GetOrCreateViewportMesh(_viewport);

        if (drawData->TotalVtxCount == 0)
            return;

        std::vector<VertexData> vertices(drawData->TotalVtxCount);
        std::vector<uint32> indices(drawData->TotalIdxCount);
        std::unordered_map<uint32, Submesh> submeshes;
            
        uint64 vertexOffset = 0;
        uint64 indexOffset = 0;
        uint32 lastSubMeshIDOffset = 0;

        Vector3 offset(drawData->DisplayPos.x, drawData->DisplayPos.y, 0.0);

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* drawList = drawData->CmdLists[n];

            for (int v = 0; v < drawList->VtxBuffer.Size; v++)
            {
                ImDrawVert* imVert = drawList->VtxBuffer.Data + v;
                VertexData& vert = vertices.at(v + vertexOffset);

                vert.Position = Vector3(imVert->pos.x, imVert->pos.y, 0.0) - offset;

                ImVec4 imGuiColor = ::ImGui::ColorConvertU32ToFloat4(imVert->col);
                Color c(imGuiColor.x, imGuiColor.y, imGuiColor.z, imGuiColor.w);
                c.ConvertToLinear();
                vert.Color = Vector4(c.R, c.G, c.B, c.A);

                vert.UV0 = Vector2(imVert->uv.x, imVert->uv.y);
            }

            for (int i = 0; i < drawList->IdxBuffer.Size; i++)
            {
                indices.at(i + indexOffset) = *(drawList->IdxBuffer.Data + i) + static_cast<uint32>(vertexOffset);
            }

            for (int cmdI = 0; cmdI < drawList->CmdBuffer.Size; cmdI++)
            {
                const ImDrawCmd& cmd = drawList->CmdBuffer[cmdI];

                uint32 subMeshID = cmdI + lastSubMeshIDOffset;
                submeshes.try_emplace(subMeshID, cmd.IdxOffset + indexOffset, cmd.ElemCount);
            }

            vertexOffset += drawList->VtxBuffer.Size;
            indexOffset += drawList->IdxBuffer.Size;
            lastSubMeshIDOffset += drawList->CmdBuffer.Size;
        }

        mesh->SetVertices(format, vertices);
        mesh->SetIndices(indices, submeshes);

        lastSubMeshIDOffset = 0;

        ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();

        for (int n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* drawList = drawData->CmdLists[n];

            for (int cmdI = 0; cmdI < drawList->CmdBuffer.Size; cmdI++)
            {
                const ImDrawCmd& cmd = drawList->CmdBuffer[cmdI];

                RectInt scissorRect(
                    Vector2Int(
                        Math::Max(static_cast<int>(cmd.ClipRect.x - drawData->DisplayPos.x), 0),
                        Math::Max(static_cast<int>(cmd.ClipRect.y - drawData->DisplayPos.y), 0)),
                    Vector2Int(
                        static_cast<int>(cmd.ClipRect.z - drawData->DisplayPos.x),
                        static_cast<int>(cmd.ClipRect.w - drawData->DisplayPos.y))
                );

                uint32 subMeshID = cmdI + lastSubMeshIDOffset;
                const Submesh* sm = nullptr;
                mesh->TryGetSubmesh(subMeshID, sm);

                RenderObjectData& obj = renderView.AddRenderObject(
                    _viewport->ID,
                    ImGuiRenderPass::VisibilityGroup,
                    mesh,
                    *sm,
                    nullptr,
                    Matrix4x4::Identity);

                obj.ExtraData = ImGuiExtraData(scissorRect, reinterpret_cast<uint64>(cmd.GetTexID()));
            }

            lastSubMeshIDOffset += drawList->CmdBuffer.Size;
        }
	}

    ResourceID ImGuiSceneDataProvider::CreateResourceIDForViewport(ImGuiViewport* viewport)
    {
        return ResourceID(FormatString("ImGuiViewport::{}", viewport->ID));
    }

    SharedRef<Mesh> ImGuiSceneDataProvider::GetOrCreateViewportMesh(ImGuiViewport* viewport)
    {
        ResourceLibrary& resources = Engine::Get()->GetResourceLibrary();
        ResourceID id = CreateResourceIDForViewport(viewport);

        SharedRef<Mesh> mesh;

        if (!resources.TryGetAs<Mesh>(id, mesh))
            mesh = resources.Create<Mesh>(id, MeshUsageFlags::Dynamic);

        return mesh;
    }
}