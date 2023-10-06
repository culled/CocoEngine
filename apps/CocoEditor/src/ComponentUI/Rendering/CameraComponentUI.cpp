#include "CameraComponentUI.h"

#include <Coco/ECS/Components/Rendering/CameraComponent.h>

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void CameraComponentUI::DrawImpl(ECS::Entity& entity)
	{
		CameraComponent& camera = entity.GetComponent<CameraComponent>();

		std::array<const char*, 2> projectionTypeNames = { "Perspective", "Orthographic" };
		int projectionIndex = static_cast<int>(camera.ProjectionType);

		if (ImGui::BeginCombo("Projection", projectionTypeNames.at(projectionIndex)))
		{
			for (int i = 0; i < projectionTypeNames.size(); i++)
			{
				bool selected = i == projectionIndex;
				if (ImGui::Selectable(projectionTypeNames.at(i), selected))
				{
					camera.ProjectionType = static_cast<CameraProjectionType>(i);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		switch (camera.ProjectionType)
		{
		case CameraProjectionType::Perspective:
		{
			float fov = static_cast<float>(Math::RadToDeg(camera.VerticalFOV));
			if (ImGui::DragFloat("Vertical FOV", &fov, 0.2f))
				camera.VerticalFOV = Math::DegToRad(fov);

			float near = static_cast<float>(camera.PerspectiveNearClip);
			if (ImGui::DragFloat("Near Clip", &near, 0.1f))
				camera.PerspectiveNearClip = near;

			float far = static_cast<float>(camera.PerspectiveFarClip);
			if (ImGui::DragFloat("Far Clip", &far, 0.1f))
				camera.PerspectiveFarClip = far;

			break;
		}
		case CameraProjectionType::Orthographic:
		{
			float size = static_cast<float>(camera.OrthoSize);
			if (ImGui::DragFloat("Vertical Size", &size, 0.2f))
				camera.OrthoSize = size;

			float near = static_cast<float>(camera.OrthoNearClip);
			if (ImGui::DragFloat("Near Clip", &near, 0.1f))
				camera.OrthoNearClip = near;

			float far = static_cast<float>(camera.OrthoFarClip);
			if (ImGui::DragFloat("Far Clip", &far, 0.1f))
				camera.OrthoFarClip = far;

			break;
		}
		default:
			break;
		}
	}
}