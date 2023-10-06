#include "ComponentUI.h"

#include <imgui.h>

namespace Coco
{
	void ComponentUI::Draw(ECS::Entity& entity)
	{
		if (ImGui::CollapsingHeader(GetHeader(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawImpl(entity);
		}
	}
}