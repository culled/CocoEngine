#include "MeshRendererComponentUI.h"
#include "../../../Panels/ViewportPanel.h"

#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/Rendering/Gizmos/Gizmos.h>
#include <Coco/Rendering/RenderService.h>

#include "../../UIUtilities.h"
#include "../../MaterialUI.h"

#include <imgui.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

namespace Coco
{
	void MeshRendererComponentUI::DrawInspectorUI(ECS::MeshRendererComponent& renderer)
	{
		uint64 visibility = renderer.GetVisibilityGroups();
		string visibilityPreview = RenderService::GetVisibilityFlagText(visibility, 2);

		if (ImGui::BeginCombo("Visibility Groups", visibilityPreview.c_str()))
		{
			for (int i = 0; i < RenderService::VisiblityGroupTexts.size(); i++)
			{
				const string& label = RenderService::VisiblityGroupTexts.at(i);
				uint64 flag = static_cast<uint64>(1) << i;
				bool selected = (visibility & flag) != 0;

				if (ImGui::Selectable(label.c_str(), &selected))
				{
					if (selected)
					{
						visibility |= flag;
					}
					else
					{
						visibility &= ~flag;
					}

					renderer.SetVisibilityGroups(visibility);
				}
			}

			ImGui::EndCombo();
		}

		SharedRef<Mesh> mesh = renderer.GetMesh();
		if (UIUtilities::DrawResourcePicker("Mesh", "Mesh", mesh))
		{
			renderer.SetMesh(mesh);
		}

		auto& materials = renderer.GetMaterials();
		for (auto& it : materials)
		{
			ImGui::PushID(it.first);

			SharedRef<Material>& materialResource = it.second;
			string t = FormatString("Material {}", it.first);
			UIUtilities::DrawResourcePicker("Material", t.c_str(), materialResource);

			if (materialResource)
			{
				if (ImGui::CollapsingHeader("Properties"))
				{
					MaterialUI::Draw(*materialResource);
					//DrawAddParameterSection(*materialResource);
					//DrawRemoveParameterSection(*materialResource);

					//SharedRef<Shader> shader;
					//if (UIUtils::DrawResourcePicker("Shader", "Add Parameters From Shader", shader))
					//{
					//	materialResource->AddParametersFromShader(*shader);
					//}
				}
			}

			ImGui::PopID();
		}
	}

	void MeshRendererComponentUI::DrawViewport3D(ViewportPanel& viewport)
	{
		SelectionContext& selectionContext = viewport.GetSelectionContext();
		const MeshRendererComponent* renderer = nullptr;
		Entity entity = selectionContext.GetSelectedEntity();
		if (entity == Entity::Null || !entity.TryGetComponent<MeshRendererComponent>(renderer))
			return;

		SharedRef<Mesh> mesh = renderer->GetMesh();
		if (!mesh)
			return;

		Matrix4x4 modelMatrix = Matrix4x4::Identity;
		const Transform3DComponent* transform = nullptr;

		if (entity.TryGetComponent<Transform3DComponent>(transform))
		{
			modelMatrix = transform->GetTransformMatrix(TransformSpace::Self, TransformSpace::Global);
		}

		Gizmos::DrawWireBounds(mesh->GetLocalBounds(), modelMatrix, Color::White);
	}
}