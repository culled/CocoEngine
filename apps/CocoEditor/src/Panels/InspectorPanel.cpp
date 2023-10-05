#include "InspectorPanel.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>
#include <Coco/ECS/Components/Transform3DComponent.h>

#include "../EditorApplication.h"
#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	InspectorPanel::InspectorPanel() :
		_selection(EditorApplication::Get()->GetSelection())
	{}

	void InspectorPanel::Update(const TickInfo& tickInfo)
	{
		if (ImGui::Begin("Inspector"))
		{
			if (_selection.HasSelectedEntity())
			{
				DrawEntityInspector();
			}
		}

		ImGui::End();
	}

	void InspectorPanel::DrawEntityInspector()
	{
		Entity& entity = _selection.GetSelectedEntity();

		if (ImGui::CollapsingHeader("Entity Info", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

			std::array<char, 256> textBuffer = { '\0' };
			Assert(strcpy_s(textBuffer.data(), textBuffer.size(), info.Name.c_str()) == 0)

			if (ImGui::InputText("Name", textBuffer.data(), textBuffer.size()))
			{
				info.Name = string(textBuffer.data());
			}
		}

		if (entity.HasComponent<Transform3DComponent>())
		{
			if (ImGui::CollapsingHeader("Transform 3D", ImGuiTreeNodeFlags_DefaultOpen))
			{
				Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();

				std::array<float, 3> position = {
					static_cast<float>(transform.Transform.LocalPosition.X),
					static_cast<float>(transform.Transform.LocalPosition.Y),
					static_cast<float>(transform.Transform.LocalPosition.Z)
				};

				if (ImGui::DragFloat3("Position", position.data(), 0.1f))
				{
					transform.Transform.LocalPosition.X = position.at(0);
					transform.Transform.LocalPosition.Y = position.at(1);
					transform.Transform.LocalPosition.Z = position.at(2);

					transform.Transform.Recalculate();
				}

				Vector3 eulerAngles = transform.Transform.LocalRotation.ToEulerAngles() * Math::Rad2DegMultiplier;

				std::array<float, 3> rotation = {
					static_cast<float>(eulerAngles.X),
					static_cast<float>(eulerAngles.Y),
					static_cast<float>(eulerAngles.Z)
				};

				if (ImGui::DragFloat3("Rotation", rotation.data(), 0.1f))
				{
					eulerAngles.X = rotation.at(0);
					eulerAngles.Y = rotation.at(1);
					eulerAngles.Z = rotation.at(2);

					transform.Transform.LocalRotation = Quaternion(eulerAngles * Math::Deg2RadMultiplier);

					transform.Transform.Recalculate();
				}

				std::array<float, 3> scale = {
					static_cast<float>(transform.Transform.LocalScale.X),
					static_cast<float>(transform.Transform.LocalScale.Y),
					static_cast<float>(transform.Transform.LocalScale.Z)
				};

				if (ImGui::DragFloat3("Scale", scale.data(), 0.1f))
				{
					transform.Transform.LocalScale.X = scale.at(0);
					transform.Transform.LocalScale.Y = scale.at(1);
					transform.Transform.LocalScale.Z = scale.at(2);

					transform.Transform.Recalculate();
				}
			}
		}
	}
}