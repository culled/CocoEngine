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

		Vector3 position = transform.GetPosition(TransformSpace::Parent);
		if (UIUtils::DrawVector3UI(entity.GetID(), "Position", position))
		{
			transform.SetPosition(position, TransformSpace::Parent);
		}

		Vector3 eulerAngles = transform.GetEulerAngles(TransformSpace::Parent) * Math::Rad2DegMultiplier;

		if (UIUtils::DrawVector3UI(entity.GetID(), "Rotation", eulerAngles))
		{
			transform.SetEulerAngles(eulerAngles * Math::Deg2RadMultiplier, TransformSpace::Parent);
		}

		Vector3 scale = transform.GetScale(TransformSpace::Parent);
		if (UIUtils::DrawVector3UI(entity.GetID(), "Scale", scale, 1.0))
		{
			transform.SetScale(scale, TransformSpace::Parent);
		}

		bool inherit = transform.GetInheritParentTransform();
		if (ImGui::Checkbox("Inherit Parent Transform", &inherit))
		{
			transform.SetInheritParentTransform(inherit, true);
		}
	}
}