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
			_scene->EachEntity([&](Entity& e) {
				DrawEntityNode(e);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				_selection.ClearSelectedEntity();
			}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity& entity)
	{
		const EntityInfoComponent& info = entity.GetComponent<EntityInfoComponent>();

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

		if (_selection.HasSelectedEntity() && _selection.GetSelectedEntity() == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool expanded = ImGui::TreeNodeEx((void*)entity.GetID(), flags, "%s", info.Name.c_str());

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			_selection.SetSelectedEntity(entity);

		if (expanded)
		{
			// TODO

			ImGui::TreePop();
		}
	}
}