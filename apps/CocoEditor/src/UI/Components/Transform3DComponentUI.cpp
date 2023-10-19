#include "Transform3DComponentUI.h"

#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/ECS/Systems/TransformSystem.h>
#include "../UIUtils.h"

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void Transform3DComponentUI::DrawPropertiesImpl(ECS::Entity& entity)
	{
		Transform3DComponent& transformComp = entity.GetComponent<Transform3DComponent>();
		Transform3D& transform = transformComp.Transform;

		if (transformComp.IsDirty)
			TransformSystem::UpdateTransform3D(entity);

		if (UIUtils::DrawVector3UI(entity.GetID(), "Position", transform.LocalPosition))
		{
			TransformSystem::MarkTransform3DDirty(entity);
		}

		Vector3 eulerAngles = transform.LocalRotation.ToEulerAngles() * Math::Rad2DegMultiplier;

		if (UIUtils::DrawVector3UI(entity.GetID(), "Rotation", eulerAngles))
		{
			transform.LocalRotation = Quaternion(eulerAngles * Math::Deg2RadMultiplier);

			TransformSystem::MarkTransform3DDirty(entity);
		}

		if (UIUtils::DrawVector3UI(entity.GetID(), "Scale", transform.LocalScale, 1.0))
		{
			TransformSystem::MarkTransform3DDirty(entity);
		}

		if (ImGui::Checkbox("Inherit Parent Transform", &transformComp.InheritParentTransform))
		{
			Vector3 globalPosition;
			Quaternion globalRotation;
			Vector3 globalScale;
			transform.GetGlobalTransform(globalPosition, globalRotation, globalScale);
			
			if (transformComp.InheritParentTransform)
			{
				Transform3DComponent* parentTransformComp;
				if (TransformSystem::TryGetParentTransform3D(entity, parentTransformComp))
				{
					Transform3D& parentTransform = parentTransformComp->Transform;

					transform.LocalPosition = parentTransform.GlobalToLocalPosition(globalPosition);
					transform.LocalRotation = parentTransform.GlobalToLocalRotation(globalRotation);
					transform.LocalScale = parentTransform.GlobalToLocalScale(globalScale);
				}				
			}
			else
			{
				transform.LocalPosition = globalPosition;
				transform.LocalRotation = globalRotation;
				transform.LocalScale = globalScale;
			}

			TransformSystem::MarkTransform3DDirty(entity);
		}
	}
}