#include "MeshRendererComponentUI.h"

#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/Rendering/Material.h>

#include <Coco/Rendering/Gizmos/GizmoRender.h>

#include <Coco/Core/Engine.h>
#include <imgui.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

namespace Coco
{
	void MeshRendererComponentUI::DrawPropertiesImpl(ECS::Entity& entity)
	{
		MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();

		//ImGui::Text("Mesh: %s", renderer.Mesh ? renderer.Mesh->GetContentPath().c_str() : "");
		string meshText = renderer.Mesh ? FormatString("Mesh: {}", renderer.Mesh->GetContentPath()) : "Mesh";
		ImGui::Button(meshText.c_str());

		auto& resources = Engine::Get()->GetResourceLibrary();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".cmesh"))
			{
				string meshPath(static_cast<const char*>(payload->Data), payload->DataSize);
				renderer.Mesh = resources.GetOrLoad<Mesh>(meshPath);
				renderer.EnsureMaterialSlots();
			}

			ImGui::EndDragDropTarget();
		}

		for (auto& it : renderer.Materials)
		{
			SharedRef<Resource> materialResource = std::dynamic_pointer_cast<Resource>(it.second);

			//ImGui::Text("Material %i: %s", it.first, materialResource ? materialResource->GetContentPath().c_str() : "");
			string t = materialResource ? FormatString("Material {}: {}", it.first, materialResource->GetContentPath()) : FormatString("Material {}", it.first);

			ImGui::PushID(it.first);

			ImGui::Button(t.c_str());

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(".cmaterial"))
				{
					string materialPath(static_cast<const char*>(payload->Data), payload->DataSize);
					it.second = resources.GetOrLoad<Material>(materialPath);
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();
		}
	}

	void MeshRendererComponentUI::DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize)
	{
		MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();

		if (!renderer.Mesh)
			return;

		GizmoRender* gizmo = GizmoRender::Get();
		if (!gizmo)
			return;

		Matrix4x4 modelMatrix = Matrix4x4::Identity;

		if (entity.HasComponent<Transform3DComponent>())
		{
			Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();
			modelMatrix = transform.Transform.GlobalTransform;
		}

		gizmo->DrawWireBounds(renderer.Mesh->GetBounds(), modelMatrix, Color::White);
	}
}