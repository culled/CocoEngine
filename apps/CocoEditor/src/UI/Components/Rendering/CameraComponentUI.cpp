#include "CameraComponentUI.h"

#include <Coco/ECS/Components/Rendering/CameraComponent.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/Core/Types/Transform.h>
#include <Coco/Rendering/Gizmos/GizmoRender.h>
#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void CameraComponentUI::DrawPropertiesImpl(ECS::Entity& entity)
	{
		CameraComponent& camera = entity.GetComponent<CameraComponent>();

		std::array<const char*, 2> projectionTypeNames = { "Perspective", "Orthographic" };
		int projectionIndex = static_cast<int>(camera.GetProjectionType());

		if (ImGui::BeginCombo("Projection", projectionTypeNames.at(projectionIndex)))
		{
			for (int i = 0; i < projectionTypeNames.size(); i++)
			{
				bool selected = i == projectionIndex;
				if (ImGui::Selectable(projectionTypeNames.at(i), selected))
				{
					camera.SetProjectionType(static_cast<CameraProjectionType>(i));
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
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

		switch (camera.GetProjectionType())
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

	void CameraComponentUI::DrawGizmosImpl(ECS::Entity& entity, const SizeInt& viewportSize)
	{
		CameraComponent& camera = entity.GetComponent<CameraComponent>();

		Vector3 position = Vector3::Zero;
		Quaternion rotation = Quaternion::Identity;

		if (entity.HasComponent<Transform3DComponent>())
		{
			const Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();
			position = transform.GetPosition(TransformSpace::Global);
			rotation = transform.GetRotation(TransformSpace::Global);
		}

		double aspect = static_cast<double>(viewportSize.Width) / viewportSize.Height;

		Rendering::GizmoRender::Get()->DrawFrustum(camera.GetViewFrustum(aspect, position, rotation), Color::White);
	}
}