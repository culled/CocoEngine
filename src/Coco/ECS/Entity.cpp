#include "Entity.h"

namespace Coco::ECS
{
	Entity::Entity() : Entity(InvalidEntityID, "Null", InvalidSceneID, InvalidEntityID)
	{}

	Entity::Entity(EntityID id, const string& name, SceneID sceneID, EntityID parentID) : _id(id), _name(name), _sceneID(sceneID), _parentID(parentID)
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