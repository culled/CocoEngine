#pragma once
#include <Coco/Core/MainLoop/TickInfo.h>
#include <Coco/ECS/Scene.h>
#include <Coco/ECS/Entity.h>

using namespace Coco::ECS;

namespace Coco
{
	class SelectionContext;

	class SceneHierarchyPanel
	{
	private:
		SharedRef<Scene> _scene;
		SelectionContext& _selection;

	public:
		SceneHierarchyPanel(SharedRef<Scene> scene);

		void SetCurrentScene(SharedRef<Scene> scene);

		void Update(const TickInfo& tickInfo);

	private:
		void DrawEntityNode(Entity& entity);
	};
}