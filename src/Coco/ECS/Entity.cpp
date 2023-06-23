#include "Entity.h"
#include "EntityManager.h"

namespace Coco::ECS
{
	Entity::Entity(EntityData& data) : _data(data)
	{}

	void Entity::Destroy()
	{
		ECSService::Get()->QueueEntityDestroy(GetID());
	}
}