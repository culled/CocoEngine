#include "Entity.h"

namespace Coco::ECS
{
	Entity::Entity() : Entity(InvalidEntityID, "Null", InvalidSceneID, InvalidEntityID)
	{}

	Entity::Entity(EntityID id, const string& name, SceneID sceneID, EntityID parentID) : _id(id), _name(name), _sceneID(sceneID), _parentID(parentID)
	{}

	void Entity::SetParentID(const EntityID& parentID)
	{
		if (parentID == _id)
			return;

		_parentID = parentID;
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