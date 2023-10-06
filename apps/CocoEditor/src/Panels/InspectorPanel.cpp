#include "InspectorPanel.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>
#include "../ComponentUI/EntityInfoComponentUI.h"
#include <Coco/ECS/Components/Transform3DComponent.h>
#include "../ComponentUI/Transform3DComponentUI.h"

#include "../EditorApplication.h"
#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	std::vector<ComponentUIContainer> InspectorPanel::_sComponentUIs = std::vector<ComponentUIContainer>();

	InspectorPanel::InspectorPanel() :
		_selection(EditorApplication::Get()->GetSelection())
	{
		RegisterDefaultComponentUIs();
	}

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

	void InspectorPanel::RegisterDefaultComponentUIs()
	{
		RegisterComponentUI<EntityInfoComponent, EntityInfoComponentUI>();
		RegisterComponentUI<Transform3DComponent, Transform3DComponentUI>();
	}

	void InspectorPanel::DrawEntityInspector()
	{
		Entity& entity = _selection.GetSelectedEntity();

		for (const ComponentUIContainer& kvp : _sComponentUIs)
		{
			if (kvp.TestFunc(entity))
			{
				kvp.UI->DrawComponent(entity);
			}
		}
	}
}