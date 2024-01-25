#include "SelectionContext.h"

using namespace Coco::ECS;

namespace Coco
{
	SelectionContext::SelectionContext() :
		_selectedEntity(Entity::Null)
	{}

	void SelectionContext::SetSelectedEntity(Entity& entity)
	{
		_selectedEntity = entity;
		OnSelectionChanged.Invoke(entity);
	}

	void SelectionContext::ClearSelectedEntity()
	{
		_selectedEntity = Entity::Null;
		OnSelectionChanged.Invoke(Entity::Null);
	}

	bool SelectionContext::HasSelectedEntity() const
	{
		return _selectedEntity != Entity::Null;
	}
}