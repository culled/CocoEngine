#include "ViewportCameraControllerComponentUI.h"

#include "../../Components/ViewportCameraControllerComponent.h"
#include "../../Panels/ViewportPanel.h"
#include <Coco/ECS/Components/Rendering/CameraComponent.h>

#include <imgui.h>

using namespace Coco::ECS;
using namespace Coco::Rendering;

namespace Coco
{
	void ViewportCameraControllerComponentUI::DrawViewportMenu(ViewportPanel& viewport)
	{
		ImGui::SeparatorText("Viewport Camera Properties");

		ECS::Entity viewportEntity = viewport.GetViewportCameraEntity();
		ViewportCameraControllerComponent& controller = viewportEntity.GetComponent<ViewportCameraControllerComponent>();

		ImGui::DragFloat("Mouse Sensitivity", &controller._lookSensitivity, 0.1f);
		ImGui::DragFloat("Move Speed", &controller._moveSpeed, 0.1f, ViewportCameraControllerComponent::MinMoveSpeed, ViewportCameraControllerComponent::MaxMoveSpeed);

		ImGui::DragFloat("Pan Speed", &controller._panSpeed, 0.1f);
		ImGui::Checkbox("Invert Pan X", &controller._invertPan.at(0));
		ImGui::SameLine();
		ImGui::Checkbox("Invert Pan Y", &controller._invertPan.at(1));

		CameraComponent& camera = viewportEntity.GetComponent<CameraComponent>();
		std::array<const char*, 4> msaaTexts = { "One", "Two", "Four", "Eight" };

		MSAASamples sampleCount = camera.GetSampleCount();

		if (ImGui::BeginCombo("MSAA samples", msaaTexts[static_cast<int>(sampleCount)]))
		{
			for (int i = 0; i < 4; i++)
			{
				MSAASamples s = static_cast<MSAASamples>(i);
				if (ImGui::Selectable(msaaTexts[i], sampleCount == s))
					sampleCount = s;

			}

			ImGui::EndCombo();
		}

		camera.SetSampleCount(sampleCount);

		float fov = static_cast<float>(Math::RadToDeg(camera.GetPerspectiveFOV()));
		if (ImGui::DragFloat("Field of View", &fov, 0.1f, 0.01f, 180.f))
		{
			camera.SetPerspectiveFOV(Math::DegToRad(fov));
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