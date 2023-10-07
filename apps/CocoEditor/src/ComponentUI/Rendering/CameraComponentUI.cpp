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

		ImGui::DragInt("Priority", &camera.Priority);

		std::array<float, 3> color = {
			static_cast<float>(camera.ClearColor.R),
			static_cast<float>(camera.ClearColor.G),
			static_cast<float>(camera.ClearColor.B),
		};

		if (ImGui::ColorEdit3("Clear Color", color.data()))
		{
			camera.ClearColor.R = color.at(0);
			camera.ClearColor.G = color.at(1);
			camera.ClearColor.B = color.at(2);
		}

		switch (camera.ProjectionType)
		{
		case CameraProjectionType::Perspective:
		{
			float fov = static_cast<float>(Math::RadToDeg(camera.PerspectiveFOV));

			if (ImGui::DragFloat("Vertical FOV", &fov, 0.2f, Math::EpsilonF))
				camera.PerspectiveFOV = Math::DegToRad(fov);

			float near = static_cast<float>(camera.PerspectiveNearClip);
			float far = static_cast<float>(camera.PerspectiveFarClip);
			if (ImGui::DragFloatRange2("Clipping Distance", &near, &far, 0.1f, Math::EpsilonF))
			{
				camera.PerspectiveNearClip = near;
				camera.PerspectiveFarClip = far;
			}

			break;
		}
		case CameraProjectionType::Orthographic:
		{
			float size = static_cast<float>(camera.OrthoSize);
			if (ImGui::DragFloat("Vertical Size", &size, 0.2f))
				camera.OrthoSize = size;

			float near = static_cast<float>(camera.OrthoNearClip);
			float far = static_cast<float>(camera.OrthoFarClip);
			if (ImGui::DragFloatRange2("Clipping Distance", &near, &far, 0.1f))
			{
				camera.OrthoNearClip = near;
				camera.OrthoFarClip = far;
			}

			break;
		}
		default:
			break;
		}
	}
}