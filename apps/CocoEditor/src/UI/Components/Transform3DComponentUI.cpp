#include "Transform3DComponentUI.h"

#include "../../Panels/ViewportPanel.h"

#include "../UIUtilities.h"
#include <imgui.h>
#include <ImGuizmo.h>

using namespace Coco::ECS;

namespace Coco
{
	void Transform3DComponentUI::DrawInspectorUI(Transform3DComponent& transform)
	{
		uint64 id = transform.GetOwner();

		Vector3 position = transform.GetPosition(TransformSpace::Parent);
		if (UIUtilities::DrawVector3UI(id, "Position", position))
		{
			transform.SetPosition(position, TransformSpace::Parent);
		}

		Vector3 eulerAngles = transform.GetEulerAngles(TransformSpace::Parent) * Math::Rad2DegMultiplier;

		if (UIUtilities::DrawVector3UI(id, "Rotation", eulerAngles))
		{
			transform.SetEulerAngles(eulerAngles * Math::Deg2RadMultiplier, TransformSpace::Parent);
		}

		Vector3 scale = transform.GetScale(TransformSpace::Parent);
		if (UIUtilities::DrawVector3UI(id, "Scale", scale, 1.0))
		{
			transform.SetScale(scale, TransformSpace::Parent);
		}

		bool inherit = transform.GetInheritParentTransform();
		if (ImGui::Checkbox("Inherit Parent Transform", &inherit))
		{
			transform.SetInheritParentTransform(inherit, true);
		}

		std::array<const char*, 2> reparentOptions = { "Keep Local Transform", "Keep World Transform" };
		int currentReparentMode = static_cast<int>(transform.GetReparentMode());

		if (ImGui::BeginCombo("Reparent Mode", reparentOptions.at(currentReparentMode)))
		{
			for (int i = 0; i < reparentOptions.size(); i++)
			{
				bool selected = i == currentReparentMode;

				if (ImGui::Selectable(reparentOptions.at(i), &selected))
				{
					transform.SetReparentMode(static_cast<TransformReparentMode>(i));
				}
			}

			ImGui::EndCombo();
		}
	}

	void Transform3DComponentUI::DrawViewport2D(ViewportPanel& viewport)
	{
		SelectionContext& selection = viewport.GetSelectionContext();
		if (!selection.HasSelectedEntity())
			return;

		Entity& selectedEntity = selection.GetSelectedEntity();

		Transform3DComponent* transform = nullptr;
		if (!selectedEntity.TryGetComponent<Transform3DComponent>(transform))
			return;

		ImGuizmo::SetID(static_cast<int>(selectedEntity));

		Matrix4x4 view, projection;
		viewport.GetViewportCameraProjection(view, projection);

		std::array<float, Matrix4x4::CellCount> projectionMat = projection.AsFloatArray();
		std::array<float, Matrix4x4::CellCount> viewMat = view.AsFloatArray();
		std::array<float, Matrix4x4::CellCount> modelMat = transform->GetTransformMatrix(TransformSpace::Self, TransformSpace::Global).AsFloatArray();

		// TODO: save these settings per viewport?
		float snapIncrement = 0.f;
		bool enableSnap = false;
		ImGuizmo::OPERATION currentOperation = ImGuizmo::OPERATION::TRANSLATE;
		ImGuizmo::MODE currentMode = ImGuizmo::MODE::LOCAL;

		std::array<float, 3> snapValues = {
				snapIncrement,
				snapIncrement,
				snapIncrement,
		};

		if (ImGuizmo::Manipulate(
			viewMat.data(),
			projectionMat.data(),
			currentOperation,
			currentMode,
			modelMat.data(),
			nullptr,
			enableSnap ? snapValues.data() : nullptr))
		{
			Matrix4x4 transformed(modelMat);

			Vector3 p, s;
			Quaternion r;
			transformed.Decompose(p, r, s);

			transform->SetTransform(&p, &r, &s, TransformSpace::Global);
		}
	}
}