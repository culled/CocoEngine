#include "SceneHierarchyPanel.h"
#include <imgui.h>

using namespace Coco::ECS;

namespace Coco
{
	SceneHierarchyPanel::SceneHierarchyPanel(SharedRef<ECS::Scene> scene, SelectionContext& selectionContext) :
		_scene(scene),
		_selectionContext(selectionContext)
	{}

	void SceneHierarchyPanel::Draw()
	{
		if (ImGui::Begin("Scene Hierarchy"))
		{
			std::vector<Entity> rootEntities = _scene->GetEntities(true);

			for (Entity& e : rootEntities)
			{
				DrawEntityNode(e);
			}

			// Left click over a black space
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			{
				_selectionContext.ClearSelectedEntity();
			}

			// Right click over a black space
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
			{
				DrawSceneContextMenu();

				ImGui::EndPopup();
			}

			ImGui::Dummy(ImGui::GetContentRegionAvail());

			//if (ImGui::BeginDragDropTarget())
			//{
			//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(Entity).name()))
			//	{
			//		ReparentEntity(payload, Entity::Null);
			//	}
			//}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetScene(SharedRef<ECS::Scene> scene)
	{
		_scene = scene;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool hasChildSelected = false;

		if (_selectionContext.HasSelectedEntity())
		{
			if (_selectionContext.GetSelectedEntity() == entity)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}
			else if (_selectionContext.GetSelectedEntity().IsDescendentOf(entity))
			{
				ImGui::SetNextItemOpen(true);
			}
		}

		std::vector<Entity> children = entity.GetChildren();

		if (children.size() == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool dimColor = !entity.IsActiveInHierarchy();
		if (dimColor)
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(128, 128, 128, 255));

		uint64 id = entity;
		bool expanded = ImGui::TreeNodeEx((void*)id, flags, "%s", entity.GetName().c_str());

		if (dimColor)
			ImGui::PopStyleColor();

		// Left click entity
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			_selectionContext.SetSelectedEntity(entity);
		}

		// Right click entity
		if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonRight))
		{
			DrawEntityContextMenu(entity);

			ImGui::EndPopup();
		}

		//if (ImGui::BeginDragDropSource())
		//{
		//	ImGui::SetDragDropPayload(typeid(Entity).name(), &id, sizeof(uint64));
		//	ImGui::EndDragDropSource();
		//}

		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(Entity).name()))
		//	{
		//		ReparentEntity(payload, entity);
		//	}
		//
		//	ImGui::EndDragDropTarget();
		//}

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
			Entity e = _scene->CreateEntity();
			_selectionContext.SetSelectedEntity(e);
		}

		//if (ImGui::MenuItem("Create Render Entity"))
		//{
		//	Entity e = _scene->CreateEntity("Render Entity");
		//	e.AddComponent<Transform3DComponent>();
		//	e.AddComponent<MeshRendererComponent>();
		//	_selection.SetSelectedEntity(e);
		//}
	}

	void SceneHierarchyPanel::DrawEntityContextMenu(Entity& entity)
	{
		if (ImGui::MenuItem("Create Empty Child"))
		{
			Entity e = _scene->CreateEntity();
			e.SetParent(entity);
			_selectionContext.SetSelectedEntity(e);
		}

		//if (ImGui::MenuItem("Create Render Child"))
		//{
		//	Entity e = _scene->CreateEntity("Render Entity");
		//	e.AddComponent<Transform3DComponent>();
		//	e.AddComponent<MeshRendererComponent>();
		//	e.SetParent(entity);
		//	_selection.SetSelectedEntity(e);
		//}

		if (ImGui::MenuItem("Clear Parent"))
		{
			entity.ClearParent();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Delete"))
		{
			_scene->DestroyEntity(entity);
			_selectionContext.ClearSelectedEntity();
		}
	}
}