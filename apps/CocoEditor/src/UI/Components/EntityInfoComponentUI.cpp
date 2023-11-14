#include "EntityInfoComponentUI.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>
#include "../UIUtils.h"

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void EntityInfoComponentUI::DrawPropertiesImpl(ECS::Entity& entity)
	{
		EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		string name = info.GetName();
		if (UIUtils::DrawInputStringEdit("Name", name, 64))
			info.SetName(name);

		bool isActive = info.GetIsActive();
		if (ImGui::Checkbox("Active", &isActive))
			info.SetActive(isActive);
	}
}