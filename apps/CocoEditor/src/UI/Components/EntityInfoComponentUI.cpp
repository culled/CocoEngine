#include "EntityInfoComponentUI.h"

#include "../UIUtilities.h"
#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void EntityInfoComponentUI::DrawInspectorUI(ECS::EntityInfoComponent& entityInfo)
	{
		uint64 id = entityInfo.GetOwner();

		string name = entityInfo.GetName();
		if (UIUtilities::DrawInputStringEdit(id, "Name", name, 64))
			entityInfo.SetName(name);

		bool isActive = entityInfo.GetIsSelfActive();
		if (ImGui::Checkbox("Active", &isActive))
			entityInfo.SetSelfActive(isActive);
	}
}