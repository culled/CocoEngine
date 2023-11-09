#include "SceneHierarchyPanel.h"

#include <Coco/ECS/Components/EntityInfoComponent.h>
#include <Coco/ECS/Components/Transform3DComponent.h>
#include <Coco/ECS/Components/Rendering/MeshRendererComponent.h>
#include <Coco/ECS/Systems/TransformSystem.h>

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
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			{
				_selection.ClearSelectedEntity();
			}

			// Right click over a black space
			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
			{
				DrawSceneContextMenu();

				ImGui::EndPopup();
			}

			ImGui::Dummy(ImGui::GetContentRegionAvail());

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(Entity).name()))
				{
					ReparentEntity(payload, Entity::Null);
				}
			}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool hasChildSelected = false;

		if (_selection.HasSelectedEntity())
		{
			if (_selection.GetSelectedEntity() == entity)
				flags |= ImGuiTreeNodeFlags_Selected;
			else if (_selection.GetSelectedEntity().IsDescendentOf(entity))
				ImGui::SetNextItemOpen(true);
		}

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

		if (ImGui::BeginDragDropSource())
		{
			EntityID id = entity.GetID();
			ImGui::SetDragDropPayload(typeid(Entity).name(), &id, sizeof(EntityID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(Entity).name()))
			{
				ReparentEntity(payload, entity);
			}

			ImGui::EndDragDropTarget();
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
			Entity e = _scene->CreateEntity();
			_selection.SetSelectedEntity(e);
		}

		if (ImGui::MenuItem("Create Render Entity"))
		{
			Entity e = _scene->CreateEntity("Render Entity");
			e.AddComponent<Transform3DComponent>();
			e.AddComponent<MeshRendererComponent>();
			_selection.SetSelectedEntity(e);
		}
	}

	void SceneHierarchyPanel::DrawEntityContextMenu(Entity& entity)
	{
		if (ImGui::MenuItem("Create Empty Child"))
		{
			Entity e = _scene->CreateEntity();
			e.SetParent(entity);
			_selection.SetSelectedEntity(e);
		}

		if (ImGui::MenuItem("Create Render Child"))
		{
			Entity e = _scene->CreateEntity("Render Entity");
			e.AddComponent<Transform3DComponent>();
			e.AddComponent<MeshRendererComponent>();
			e.SetParent(entity);
			_selection.SetSelectedEntity(e);
		}

		if (ImGui::MenuItem("Clear Parent"))
		{
			entity.ClearParent();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Delete"))
		{
			_scene->DestroyEntity(entity);
			_selection.ClearSelectedEntity();
		}
	}

	void SceneHierarchyPanel::ReparentEntity(const ImGuiPayload* dragDropPayload, const Entity& parent)
	{
		EntityID id;
		Assert(memcpy_s(&id, sizeof(EntityID), dragDropPayload->Data, dragDropPayload->DataSize) == 0)
		Entity child;
		Assert(_scene->TryGetEntity(id, child))

		Transform3DComponent* transformComp = nullptr;
		Transform3D* transform = nullptr;
		Vector3 p, s;
		Quaternion r;

		if (child.TryGetComponent(transformComp))
		{
			transform = &transformComp->Transform;
			transform->GetGlobalTransform(p, r, s);
		}

		if (parent == Entity::Null)
		{
			child.ClearParent();
		}
		else
		{
			child.SetParent(parent);

			Transform3DComponent* parentTransformComp;
			if (parent.TryGetComponent(parentTransformComp))
			{
				parentTransformComp->Transform.TransformGlobalToLocal(p, r, s);
			}
		}

		if (transform)
		{
			transform->LocalPosition = p;
			transform->LocalRotation = r;
			transform->LocalScale = s;
		}
	}
}