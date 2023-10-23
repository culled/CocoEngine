#include "SelectionContext.h"

namespace Coco
{
	SelectionContext::SelectionContext() :
		_selectedEntity(Entity::Null)
	{
	}

	void SelectionContext::SetSelectedEntity(Entity& entity)
	{
		_selectedEntity = entity;
	}

	void SelectionContext::ClearSelectedEntity()
	{
		_selectedEntity = Entity::Null;
	}

	bool SelectionContext::HasSelectedEntity() const
	{
		return _selectedEntity != Entity::Null;
	}
}