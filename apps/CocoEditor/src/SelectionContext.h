#pragma once

#include <Coco/ECS/Entity.h>

using namespace Coco::ECS;

namespace Coco
{
	class SelectionContext
	{
	private:
		Entity _selectedEntity;

	public:
		SelectionContext();

		void SetSelectedEntity(Entity& entity);
		void ClearSelectedEntity();

		bool HasSelectedEntity() const;
		Entity& GetSelectedEntity() { return _selectedEntity; }
		const Entity& GetSelectedEntity() const { return _selectedEntity; }
	};
}