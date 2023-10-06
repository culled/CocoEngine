#include "EntityInfoComponentUI.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	void EntityInfoComponentUI::DrawComponent(ECS::Entity& entity)
	{
		if (ImGui::CollapsingHeader("Entity Info", ImGuiTreeNodeFlags_DefaultOpen))
		{
			EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

			std::array<char, 256> textBuffer = { '\0' };
			Assert(strcpy_s(textBuffer.data(), textBuffer.size(), info.Name.c_str()) == 0)

				if (ImGui::InputText("Name", textBuffer.data(), textBuffer.size()))
				{
					info.Name = string(textBuffer.data());
				}
		}
	}
}