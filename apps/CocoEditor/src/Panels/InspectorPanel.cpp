#include "InspectorPanel.h"

#include "../UI/ComponentUI.h"
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

		ComponentUI::DrawProperties(entity);
	}
}