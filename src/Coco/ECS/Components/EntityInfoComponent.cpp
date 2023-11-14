#include "ECSpch.h"
#include "EntityInfoComponent.h"

namespace Coco::ECS
{
	EntityInfoComponent::EntityInfoComponent(const Entity& owner) :
		EntityInfoComponent(owner, "Entity", InvalidEntityID)
	{}

	EntityInfoComponent::EntityInfoComponent(const Entity& owner, const string& name, const EntityID& entityID) :
		EntityComponent(owner),
		_name(name),
		_entityID(entityID),
		_isActive(true),
		_isActiveInHierarchy(true)
	{}

	void EntityInfoComponent::SetName(const string& name)
	{
		_name = name;
	}

	void EntityInfoComponent::SetActive(bool active)
	{
		_isActive = active;

		if (_isActive == _isActiveInHierarchy)
			return;

		bool visibleInScene = _isActive;

		const Entity& entity = GetOwner();
		if (visibleInScene && entity.HasParent())
		{
			// We aren't visible in the scene if our parent isn't visible
			const Entity& parent = entity.GetParent();
			visibleInScene = parent.GetComponent<EntityInfoComponent>()._isActiveInHierarchy;
		}

		UpdateSceneVisibility(visibleInScene);
	}

	void EntityInfoComponent::UpdateSceneVisibility(bool isVisible)
	{
		// Don't continue down the tree if we're at the proper state.
		// We assume our children are always in the same state as the parent
		if (_isActiveInHierarchy == isVisible)
			return;

		_isActiveInHierarchy = isVisible;

		const Entity& entity = GetOwner();
		for (const Entity& child : entity.GetChildren())
		{
			EntityInfoComponent& childInfo = child.GetComponent<EntityInfoComponent>();
			childInfo.UpdateSceneVisibility(isVisible);
		}
	}
}