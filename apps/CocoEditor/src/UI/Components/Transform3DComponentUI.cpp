#include "Transform3DComponentUI.h"

#include <Coco/ECS/Components/Transform3DComponent.h>
#include "../UIUtils.h"

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void Transform3DComponentUI::DrawPropertiesImpl(ECS::Entity& entity)
	{
		Transform3DComponent& transform = entity.GetComponent<Transform3DComponent>();

		if (UIUtils::DrawVector3UI(entity.GetID(), "Position", transform.Transform.LocalPosition))
		{
			transform.Transform.Recalculate();
		}

		Vector3 eulerAngles = transform.Transform.LocalRotation.ToEulerAngles() * Math::Rad2DegMultiplier;

		if (UIUtils::DrawVector3UI(entity.GetID(), "Rotation", eulerAngles))
		{
			transform.Transform.LocalRotation = Quaternion(eulerAngles * Math::Deg2RadMultiplier);

			transform.Transform.Recalculate();
		}

		if (UIUtils::DrawVector3UI(entity.GetID(), "Scale", transform.Transform.LocalScale, 1.0))
		{
			transform.Transform.Recalculate();
		}
	}
}