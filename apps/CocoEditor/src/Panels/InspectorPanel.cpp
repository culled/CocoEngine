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
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component");

		Entity& entity = _selection.GetSelectedEntity();

		ComponentUI::DrawProperties(entity);

		// Right click over a black space
		if (ImGui::BeginPopupContextWindow("Add Component", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			ImGui::SeparatorText("Add Component");

			if (ComponentUI::DrawAddComponentUI(entity))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}
}