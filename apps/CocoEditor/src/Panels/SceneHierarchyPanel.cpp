#include "SceneHierarchyPanel.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>

#include "../EditorApplication.h"
#include <imgui.h>

namespace Coco
{
	SceneHierarchyPanel::SceneHierarchyPanel(SharedRef<Scene> scene) :
		_scene(scene),
		_selection(EditorApplication::Get()->GetSelection())
	{}

	void SceneHierarchyPanel::SetCurrentScene(SharedRef<Scene> scene)
	{
		_scene = scene;
	}

	void SceneHierarchyPanel::Update(const TickInfo& tickInfo)
	{
		if (ImGui::Begin("Scene Hierarchy"))
		{
			std::vector<Entity> rootEntities = _scene->GetRootEntities();

			for (Entity& e : rootEntities)
			{
				DrawEntityNode(e);
			}

			// Left click over a black space
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				_selection.ClearSelectedEntity();
			}

			// Right click over a black space
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
			{
				DrawSceneContextMenu();

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (_selection.HasSelectedEntity() && _selection.GetSelectedEntity() == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		std::vector<Entity> children = entity.GetChildren();

		if (children.size() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool expanded = ImGui::TreeNodeEx((void*)entity.GetID(), flags, "%s", info.Name.c_str());

		// Left click entity
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			_selection.SetSelectedEntity(entity);

		// Right click entity
		if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight))
		{
			DrawEntityContextMenu(entity);
		
			ImGui::EndPopup();
		}

		if (expanded)
		{
			for (Entity& child : children)
				DrawEntityNode(child);

			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawSceneContextMenu()
	{
		if (ImGui::MenuItem("Create Empty Entity"))
		{
			_scene->CreateEntity();
		}
	}

	void SceneHierarchyPanel::DrawEntityContextMenu(Entity& entity)
	{
		if (ImGui::MenuItem("Delete"))
		{
			_scene->DestroyEntity(entity);
			_selection.ClearSelectedEntity();
		}
	}
}