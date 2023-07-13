#include "Entity.h"
#include "ECSService.h"

namespace Coco::ECS
{
	Entity::Entity() : Entity(InvalidEntityID, "Null", InvalidSceneID, InvalidEntityID)
	{}

	Entity::Entity(const EntityID& id, const string& name, SceneID sceneID, const EntityID& parentID) : ID(id), _name(name), _sceneID(sceneID), _parentID(parentID)
	{}

	void Entity::SetParentID(const EntityID& parentID)
	{
		if (parentID == ID)
			return;

		_parentID = parentID;
	}

	bool Entity::TryGetParent(Entity*& parent)
	{
		return ECSService::Get()->TryGetEntity(_parentID, parent);
	}

	List<Entity*> Entity::GetChildren() const
	{
		return ECSService::Get()->GetEntityChildren(ID);
	}
}