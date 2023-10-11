#include "MeshRendererComponentUI.h"

#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>

#include <imgui.h>

using namespace Coco::ECS;

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
	}
}