#pragma once

#include <Coco/Core/Events/Event.h>
#include <Coco/ECS/Entity.h>

namespace Coco
{
	class SelectionContext
	{
	public:
		Event<ECS::Entity> OnSelectionChanged;

	public:
		SelectionContext();

		void SetSelectedEntity(ECS::Entity& entity);
		void ClearSelectedEntity();

		bool HasSelectedEntity() const;
		ECS::Entity& GetSelectedEntity() { return _selectedEntity; }
		const ECS::Entity& GetSelectedEntity() const { return _selectedEntity; }

	private:
		ECS::Entity _selectedEntity;
	};
}