#include "MeshRendererComponentUI.h"

#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>
#include <Coco/ECS/Components/Transform3DComponent.h>

#include <Coco/Rendering/Gizmos/GizmoRender.h>

#include <imgui.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

namespace Coco
{
	void MeshRendererComponentUI::DrawPropertiesImpl(ECS::Entity& entity)
	{
		MeshRendererComponent& renderer = entity.GetComponent<MeshRendererComponent>();

		ImGui::Text("Mesh Path: %s", renderer.Mesh ? renderer.Mesh->GetContentPath().c_str() : "");

		for (const auto& it : renderer.Materials)
		{
			SharedRef<Resource> materialResource = std::dynamic_pointer_cast<Resource>(it.second);

			ImGui::Text("Material %i: %s", it.first, materialResource ? materialResource->GetContentPath().c_str() : "");
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