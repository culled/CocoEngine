#pragma once

#include <Coco/Core/Events/Event.h>
#include <Coco/ECS/Entity.h>

using namespace Coco::ECS;

namespace Coco
{
	class SelectionContext
	{
	public:
		Event<Entity> OnSelectionChanged;

	public:
		SelectionContext();

		void SetSelectedEntity(Entity& entity);
		void ClearSelectedEntity();

		bool HasSelectedEntity() const;
		Entity& GetSelectedEntity() { return _selectedEntity; }
		const Entity& GetSelectedEntity() const { return _selectedEntity; }

	private:
		Entity _selectedEntity;
	};
}