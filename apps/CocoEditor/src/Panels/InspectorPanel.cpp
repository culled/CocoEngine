#include "InspectorPanel.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>

#include "../EditorApplication.h"
#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	InspectorPanel::InspectorPanel() :
		_selection(EditorApplication::Get()->GetSelection())
	{}

	void InspectorPanel::Update(const TickInfo& tickInfo)
	{
		if (ImGui::Begin("Inspector"))
		{
			if (_selection.HasSelectedEntity())
			{
				DrawEntityInspector();
			}
		}

		ImGui::End();
	}

	void InspectorPanel::DrawEntityInspector()
	{
		Entity& entity = _selection.GetSelectedEntity();

		EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		std::array<char, 256> textBuffer = { '\0' };
		Assert(strcpy_s(textBuffer.data(), textBuffer.size(), info.Name.c_str()) == 0)

		if (ImGui::InputText("Name", textBuffer.data(), textBuffer.size()))
		{
			info.Name = string(textBuffer.data());
		}
	}
}