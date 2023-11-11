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

		UIUtils::DrawInputStringEdit("Name", info.Name, 64);

		ImGui::Checkbox("Active", &info.IsActive);
	}
}