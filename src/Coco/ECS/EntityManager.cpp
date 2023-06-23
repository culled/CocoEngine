#include "EntityManager.h"
#include "Entity.h"

namespace Coco::ECS
{
	EntityManager::EntityManager()
	{}

	EntityManager::~EntityManager()
	{
	}

	Ref<Entity> EntityManager::CreateEntity(string name)
	{
		EntityID nextEntityID = _entityIDCounter++;

		auto result = _entities.try_emplace(nextEntityID, nextEntityID, name);

		return CreateEntityWrapper((*result.first).second);
	}

	Ref<Entity> EntityManager::GetEntity(EntityID entityID)
	{
		return CreateEntityWrapper(_entities.at(entityID));
	}
	
	bool EntityManager::TryGetEntity(EntityID entityID, Ref<Entity>& entity)
	{
		const auto it = _entities.find(entityID);

		if (it != _entities.end())
		{
			entity = CreateEntityWrapper((*it).second);
			return true;
		}

		entity = nullptr;
		return false;
	}

	void EntityManager::QueueEntityDestroy(EntityID entityID)
	{
		_queuedEntitiesToDestroy.Add(entityID);
	}

	Ref<Entity> EntityManager::CreateEntityWrapper(EntityData& data)
	{
		return CreateRef<Entity>(data);
	}

	void EntityManager::DestroyQueuedEntities()
	{
		for (const auto& entityID : _queuedEntitiesToDestroy)
		{
			DestroyEntity(entityID);
		}

		_queuedEntitiesToDestroy.Clear();
	}

	void EntityManager::DestroyEntity(EntityID entityID)
	{
		_entities.erase(entityID);
		EntityDestroyed.Invoke(entityID);
	}
}
