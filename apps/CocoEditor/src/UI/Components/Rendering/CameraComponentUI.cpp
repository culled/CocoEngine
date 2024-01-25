#include "CameraComponentUI.h"

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void CameraComponentUI::DrawInspectorUI(ECS::CameraComponent& camera)
	{
		std::array<const char*, 2> projectionTypeNames = { "Perspective", "Orthographic" };

		CameraProjectionType projectionType = camera.GetProjectionType();
		int projectionIndex = static_cast<int>(projectionType);

		if (ImGui::BeginCombo("Projection Type", projectionTypeNames.at(projectionIndex)))
		{
			for (int i = 0; i < projectionTypeNames.size(); i++)
			{
				bool selected = i == projectionIndex;

				if (ImGui::Selectable(projectionTypeNames.at(i), selected))
				{
					camera.SetProjectionType(static_cast<CameraProjectionType>(i));
				}
			}

			ImGui::EndCombo();
		}

		int priority = camera.GetPriority();
		if (ImGui::DragInt("Priority", &priority))
			camera.SetPriority(priority);

		std::array<float, 4> color = camera.GetClearColor().AsFloatArray(false);
		if (ImGui::ColorEdit3("Clear Color", color.data()))
		{
			camera.SetClearColor(Color(color, false));
		}

		switch (projectionType)
		{
		case CameraProjectionType::Perspective:
		{
			float fov = static_cast<float>(Math::RadToDeg(camera.GetPerspectiveFOV()));

			if (ImGui::DragFloat("Vertical FOV", &fov, 0.2f, Math::EpsilonF))
				camera.SetPerspectiveFOV(Math::DegToRad(fov));

			break;
		}
		case CameraProjectionType::Orthographic:
		{
			float size = static_cast<float>(camera.GetOrthographicSize());
			if (ImGui::DragFloat("Vertical Size", &size, 0.2f))
				camera.SetOrthographicSize(size);

			break;
		}
		default:
			break;
		}

		float near = static_cast<float>(camera.GetNearClip());
		float far = static_cast<float>(camera.GetFarClip());
		if (ImGui::DragFloatRange2("Clipping Distance", &near, &far, 0.1f, Math::EpsilonF))
		{
			camera.SetNearClip(near);
			camera.SetFarClip(far);
		}
	}
}