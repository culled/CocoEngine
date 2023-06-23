#pragma once

#include <Coco/Core/Services/EngineService.h>
#include "EntityManager.h"
#include "EntityComponentManager.h"

namespace Coco::ECS
{
	class ECSService : public EngineService
	{
	private:
		static ECSService* _instance;

		Managed<EntityManager> _entityManager;
		Managed<EntityComponentManager> _componentManager;

	public:
		/// @brief Priority for the tick handling entities
		const static int ProcessTickPriority = -20000;

		ECSService(EngineServiceManager* serviceManager);

		static ECSService* Get() { return _instance; }

		EntityManager* GetEntityManager() { return _entityManager.get(); }
		EntityComponentManager* GetComponentManager() { return _componentManager.get(); }

		Ref<Entity> CreateEntity(string name = "") { return _entityManager->CreateEntity(name); }
		Ref<Entity> GetEntity(EntityID entityID) { return _entityManager->GetEntity(entityID); }
		bool TryGetEntity(EntityID entityID, Ref<Entity>& entity) { return _entityManager->TryGetEntity(entityID, entity); }
		void QueueEntityDestroy(EntityID entityID) { _entityManager->QueueEntityDestroy(entityID); }

		template<typename ComponentType, typename ... Args>
		ComponentType* AddComponent(EntityID entity, Args&& ... args)
		{
			return _componentManager->AddComponent<ComponentType>(entity, std::forward(args)...);
		}

		template<typename ComponentType>
		ComponentType* GetComponent(EntityID entity)
		{
			return _componentManager->GetComponent<ComponentType>(entity);
		}

		template<typename ComponentType>
		bool HasComponent(EntityID entity)
		{
			return _componentManager->HasComponent<ComponentType>(entity);
		}

		template<typename ComponentType>
		void RemoveComponent(EntityID entity)
		{
			_componentManager->RemoveComponent<ComponentType>(entity);
		}

		template<typename ComponentType>
		List<EntityID> GetEntitiesWithComponent()
		{
			return _componentManager->GetEntitiesWithComponent<ComponentType>();
		}

	private:
		/// @brief Tick for updating any entities
		/// @param deltaTime The number of seconds since the last tick
		void Process(double deltaTime);
	};
}