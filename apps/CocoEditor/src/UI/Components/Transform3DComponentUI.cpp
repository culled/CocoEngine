#include "Transform3DComponentUI.h"

#include "../UIUtilities.h"
#include <imgui.h>

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
}