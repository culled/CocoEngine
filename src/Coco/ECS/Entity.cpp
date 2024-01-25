#include "ECSpch.h"
#include "Entity.h"

#include "Components/EntityInfoComponent.h"

#include <Coco/Core/Engine.h>
#include "entt.h"

namespace Coco::ECS
{
	const Entity Entity::Null = Entity();

	Entity::Entity() :
		_handle(entt::null)
	{}

	Entity::Entity(const Entity& other) :
		_handle(other._handle)
	{}

	Entity::Entity(entt::entity handle) :
		_handle(handle)
	{}

	bool Entity::operator==(const Entity& other) const
	{
		if (_handle != other._handle)
			return false;

		// Same handle, so check if both are null handles
		return _handle == entt::null;
	}

	bool Entity::IsValid() const
	{
		return ECSService::Get()->IsEntityValid(*this);
	}

	void Entity::SetParent(const Entity& parent)
	{
		ECSService::Get()->SetEntityParent(*this, parent);
	}

	void Entity::ClearParent()
	{
		ECSService::Get()->ClearEntityParent(*this);
	}

	std::vector<Entity> Entity::GetChildren() const
	{
		return ECSService::Get()->GetEntityChildren(*this);
	}

	Entity Entity::GetParent() const
	{
		return ECSService::Get()->GetEntityParent(*this);
	}

	bool Entity::IsOrphaned() const
	{
		return ECSService::Get()->IsEntityOrphaned(*this);
	}

	bool Entity::IsDecendentOf(const Entity& ancestor) const
	{
		return ECSService::Get()->IsEntityDescendentOf(*this, ancestor);
	}

	bool Entity::IsActiveInHierarchy() const
	{
		return GetComponent<EntityInfoComponent>().IsActiveInHierarchy();
	}
}