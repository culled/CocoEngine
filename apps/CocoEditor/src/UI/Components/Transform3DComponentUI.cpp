#include "Transform3DComponentUI.h"

#include "../../Panels/ViewportPanel.h"

#include "../UIUtilities.h"
#include <imgui.h>
#include <ImGuizmo.h>

using namespace Coco::ECS;

namespace Coco
{
	ViewportTransformSettings::ViewportTransformSettings() :
		SnapIncrementMove(1.f),
		SnapIncrementRotate(45.f),
		SnapIncrementScale(1.f),
		EnableSnap(false),
		CurrentOperation(ImGuizmo::OPERATION::TRANSLATE),
		CurrentMode(ImGuizmo::MODE::LOCAL)
	{}

	std::unordered_map<uint64, ViewportTransformSettings> Transform3DComponentUI::_settings;

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

	bool Transform3DComponentUI::DrawViewport2D(ViewportPanel& viewport)
	{
		SelectionContext& selection = viewport.GetSelectionContext();
		if (!selection.HasSelectedEntity())
			return false;

		Entity& selectedEntity = selection.GetSelectedEntity();

		Transform3DComponent* transform = nullptr;
		if (!selectedEntity.TryGetComponent<Transform3DComponent>(transform))
			return false;

		ImGuizmo::SetID(static_cast<int>(selectedEntity));

		Matrix4x4 view, projection;
		viewport.GetViewportCameraProjection(view, projection);

		std::array<float, Matrix4x4::CellCount> projectionMat = projection.AsFloatArray();
		std::array<float, Matrix4x4::CellCount> viewMat = view.AsFloatArray();
		std::array<float, Matrix4x4::CellCount> modelMat = transform->GetTransformMatrix(TransformSpace::Self, TransformSpace::Global).AsFloatArray();

		ViewportTransformSettings& settings = _settings[viewport.GetID()];
		float snapIncrement = 0.f;

		switch (settings.CurrentOperation)
		{
		case ImGuizmo::OPERATION::TRANSLATE:
			snapIncrement = settings.SnapIncrementMove;
			break;
		case ImGuizmo::OPERATION::ROTATE:
			snapIncrement = settings.SnapIncrementRotate;
			break;
		case ImGuizmo::OPERATION::SCALE:
			snapIncrement = settings.SnapIncrementScale;
			break;
		default:
			break;
		}

		std::array<float, 3> snapValues = {
				snapIncrement,
				snapIncrement,
				snapIncrement,
		};

		if (ImGuizmo::Manipulate(
			viewMat.data(),
			projectionMat.data(),
			settings.CurrentOperation,
			settings.CurrentMode,
			modelMat.data(),
			nullptr,
			settings.EnableSnap ? snapValues.data() : nullptr))
		{
			Matrix4x4 transformed(modelMat);

			Vector3 p, s;
			Quaternion r;
			transformed.Decompose(p, r, s);

			transform->SetTransform(&p, &r, &s, TransformSpace::Global);
		}

		return ImGuizmo::IsOver();
	}

	void Transform3DComponentUI::DrawViewportMenu(ViewportPanel& viewport)
	{
		ViewportTransformSettings& settings = _settings[viewport.GetID()];

		int operationV = static_cast<int>(settings.CurrentOperation);

		ImGui::RadioButton("Translate", &operationV, static_cast<int>(ImGuizmo::OPERATION::TRANSLATE));
		ImGui::RadioButton("Rotate", &operationV, static_cast<int>(ImGuizmo::OPERATION::ROTATE));
		ImGui::RadioButton("Scale", &operationV, static_cast<int>(ImGuizmo::OPERATION::SCALE));

		settings.CurrentOperation = static_cast<ImGuizmo::OPERATION>(operationV);

		ImGui::Separator();

		int modeV = static_cast<int>(settings.CurrentMode);

		ImGui::RadioButton("Global", &modeV, static_cast<int>(ImGuizmo::MODE::WORLD));
		ImGui::RadioButton("Local", &modeV, static_cast<int>(ImGuizmo::MODE::LOCAL));

		settings.CurrentMode = static_cast<ImGuizmo::MODE>(modeV);

		ImGui::Separator();

		ImGui::Checkbox("Snap", &settings.EnableSnap);
	}

	void Transform3DComponentUI::DrawViewportSnapSettingsMenu(ViewportPanel& viewport)
	{
		ViewportTransformSettings& settings = _settings[viewport.GetID()];

		ImGui::DragFloat("Move Increment", &settings.SnapIncrementMove, 0.1f, 0.001f);
		ImGui::DragFloat("Rotation Increment", &settings.SnapIncrementRotate, 1.f, 0.001f, 180.f);
		ImGui::DragFloat("Scale Increment", &settings.SnapIncrementScale, 0.1f, 0.001f);
	}
}