#include "ECSpch.h"
#include "EntityInfoComponent.h"

namespace Coco::ECS
{
	EntityInfoComponent::EntityInfoComponent(const Entity& owner) :
		EntityInfoComponent(owner, "Entity", nullptr)
	{}

	EntityInfoComponent::EntityInfoComponent(const Entity& owner, const string& name, const SharedRef<Scene>& scene) :
		EntityComponent(owner),
		_name(name),
		_scene(scene),
		_isSelfActive(true),
		_isActiveInHierarchy(true)
	{}

	void EntityInfoComponent::SetName(const string& name)
	{
		_name = name;
	}

	void EntityInfoComponent::SetSelfActive(bool isActive)
	{
		_isSelfActive = isActive;

		if (_isSelfActive == _isActiveInHierarchy)
			return;

		bool parentVisible = true;
		const Entity& parent = GetOwner().GetParent();

		if (parent != Entity::Null)
		{
			// We aren't visible in the scene if our parent isn't visible
			parentVisible = parent.GetComponent<EntityInfoComponent>()._isActiveInHierarchy;
		}

		UpdateSceneVisibility(parentVisible);
	}

	void EntityInfoComponent::SetScene(SharedRef<Scene> scene)
	{
		_scene = scene;
	}

	void EntityInfoComponent::UpdateSceneVisibility(bool isParentVisible)
	{
		bool isVisible = isParentVisible && _isSelfActive;

		// Don't continue down the tree if we're at the proper state.
		// We assume our children are always in the same state as the parent
		if (_isActiveInHierarchy == isVisible)
			return;

		_isActiveInHierarchy = isVisible;

		const Entity& entity = GetOwner();
		for (Entity& child : entity.GetChildren())
		{
			EntityInfoComponent& childInfo = child.GetComponent<EntityInfoComponent>();
			childInfo.UpdateSceneVisibility(isVisible);
		}
	}
}