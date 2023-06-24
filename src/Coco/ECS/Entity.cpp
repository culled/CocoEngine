#include "Entity.h"

namespace Coco::ECS
{
	Entity::Entity() : Entity(InvalidEntityID, "Null", InvalidEntityID)
	{}

	Entity::Entity(EntityID id, const string& name, EntityID parentID) : _id(id), _name(name), _parentID(parentID)
	{}

	void Entity::SetParent(const Entity& parent)
	{
		if (parent == *this)
			return;

		_parentID = parent._id;
	}

	bool Entity::TryGetParent(Entity*& parent)
	{
		return ECSService::Get()->TryGetEntity(_parentID, parent);
	}

	List<Entity*> Entity::GetChildren() const
	{
		return ECSService::Get()->GetEntityChildren(_id);
	}
}