#pragma once

#include <Coco/ECS/Entity.h>
#include <Coco/ECS/Scene.h>
#include "../SelectionContext.h"

namespace Coco
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel(SharedRef<ECS::Scene> scene, SelectionContext& selectionContext);

		void Draw();
		void SetScene(SharedRef<ECS::Scene> scene);

	private:
		SharedRef<ECS::Scene> _scene;
		SelectionContext& _selectionContext;

	private:
		void DrawEntityNode(ECS::Entity& entity);
		void DrawSceneContextMenu();
		void DrawEntityContextMenu(ECS::Entity& entity);
	};
}