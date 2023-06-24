#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/MemoryPool.h>
#include <Coco/Core/Types/Set.h>
#include <Coco/Core/Types/Map.h>
#include "EntityTypes.h"
#include "EntityComponentList.h"
#include <type_traits>

namespace Coco::ECS
{
	class Entity;

	class ECSService : public EngineService
	{
	public:
		static constexpr uint64_t MaxEntities = 10000;

	private:
		static ECSService* _instance;
		Managed<MappedMemoryPool<Entity, MaxEntities>> _entities;
		Set<EntityID> _queuedEntitiesToDestroy;
		UnorderedMap<const char*, Managed<IEntityComponentList>> _componentLists;


	public:
		/// @brief Priority for the tick handling entities
		const static int ProcessTickPriority = -20000;

		ECSService(EngineServiceManager* serviceManager);
		~ECSService();

		static ECSService* Get() { return _instance; }

		EntityID CreateEntity(const string& name = "", EntityID parentID = InvalidEntityID);

		Entity& GetEntity(EntityID entityID);
		bool TryGetEntity(EntityID entityID, Entity*& entity);

		List<EntityID> GetEntityChildrenIDs(EntityID entity);
		List<Entity*> GetEntityChildren(EntityID entity);

		Entity& GetEntityParent(EntityID entityID);
		bool TryGetEntityParent(EntityID entityID, Entity*& entity);

		void QueueEntityDestroy(EntityID entityID);

		template<typename ComponentType, typename ... Args>
		ComponentType& AddComponent(EntityID entityID, Args&& ... args)
		{
			return GetComponentList<ComponentType>()->AddComponent(entityID, std::forward<Args>(args)...);
		}

		template<typename ComponentType>
		ComponentType& GetComponent(EntityID entityID)
		{
			return GetComponentList<ComponentType>()->GetComponent(entityID);
		}

		template<typename ComponentType>
		bool HasComponent(EntityID entityID) const
		{
			return GetComponentList<ComponentType>()->HasComponent(entityID);
		}

		template<typename ComponentType>
		bool RemoveComponent(EntityID entityID)
		{
			return GetComponentList<ComponentType>()->RemoveComponent(entityID);
		}

	private:
		/// @brief Tick for updating any entities
		/// @param deltaTime The number of seconds since the last tick
		void Process(double deltaTime);

		template<typename ComponentType>
		EntityComponentList<ComponentType, MaxEntities>* GetComponentList()
		{
			const char* key = typeid(ComponentType).name();

			if (!_componentLists.contains(key))
			{
				_componentLists.try_emplace(key, CreateManaged<EntityComponentList<ComponentType, MaxEntities>>());
			}

			return static_cast<EntityComponentList<ComponentType, MaxEntities>*>(_componentLists.at(key).get());
		}
	};
}