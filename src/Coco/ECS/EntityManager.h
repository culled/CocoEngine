#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Events/Event.h>
#include "EntityData.h"

namespace Coco::ECS
{
	class Entity;

	class EntityManager
	{
	public:
		Event<EntityID> EntityDestroyed;

	public:
		EntityManager();
		~EntityManager();

		Ref<Entity> CreateEntity(string name = "");
		Ref<Entity> GetEntity(EntityID entityID);
		bool TryGetEntity(EntityID entityID, Ref<Entity>& entity);
		void QueueEntityDestroy(EntityID entityID);

	private:
		Ref<Entity> CreateEntityWrapper(EntityData& data);

		void DestroyQueuedEntities();
		void DestroyEntity(EntityID entityID);

	private:
		Map<EntityID, EntityData> _entities;
		List<EntityID> _queuedEntitiesToDestroy;

		std::atomic<EntityID> _entityIDCounter;

		friend class Entity;
		friend class ECSService;
	};
}