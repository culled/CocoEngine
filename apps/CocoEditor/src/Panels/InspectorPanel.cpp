#include "InspectorPanel.h"

#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	std::vector<InspectorPanel::ComponentInfo> InspectorPanel::_registeredComponents;

	InspectorPanel::InspectorPanel(SelectionContext& selectionContext) :
		_selectionContext(selectionContext)
	{}

	void InspectorPanel::Draw()
	{
		if (ImGui::Begin("Inspector"))
		{
			if (_selectionContext.HasSelectedEntity())
			{
				DrawEntityInspector(_selectionContext.GetSelectedEntity());
			}
		}

		ImGui::End();
	}

	void InspectorPanel::DrawEntityInspector(Entity& entity)
	{
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("Add Component");

		bool addMargin = false;

		for (auto& componentInfo : _registeredComponents)
		{
			if (componentInfo.ComponentTestFunction(entity))
			{
				// Add margin below previous components
				if(addMargin)
				{
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 16.0f);
				}

				float availableWidth = ImGui::GetContentRegionAvail().x;
				bool open = ImGui::CollapsingHeader(componentInfo.HeaderText.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
				bool remove = false;

				ImGui::PushID(componentInfo.HeaderText.c_str());

				float lineHeight = ImGui::GetFrameHeight();
				const float buttonWidth = 32.f;
				ImGui::SameLine(availableWidth - buttonWidth + 12.f);

				if (ImGui::Button("...", ImVec2{ buttonWidth, lineHeight }))
				{
					ImGui::OpenPopup("##ComponentSettings");
				}

				if (ImGui::BeginPopup("##ComponentSettings"))
				{
					if (componentInfo.IsRemovable && ImGui::MenuItem("Remove"))
					{
						remove = true;
					}

					ImGui::EndPopup();
				}

				addMargin = open;

				if (open)
				{
					componentInfo.DrawFunction(entity);
				}

				if (remove)
				{
					componentInfo.ComponentRemovalFunction(entity);
				}

				ImGui::PopID();
			}
		}

		// Right click over a black space
		if (ImGui::BeginPopupContextWindow("Add Component", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
		{
			ImGui::SeparatorText("Add Component");

			for (auto& componentInfo : _registeredComponents)
			{
				// Don't display items for components that the entity already has
				if (componentInfo.ComponentTestFunction(entity))
					continue;

				if (ImGui::MenuItem(componentInfo.HeaderText.c_str()))
				{
					componentInfo.ComponentAdditionFunction(entity);
				}
			}

			ImGui::EndPopup();
		}
	}
}