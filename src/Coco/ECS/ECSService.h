#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/MemoryPool.h>
#include <Coco/Core/Types/Set.h>
#include <Coco/Core/Types/Map.h>
#include "EntityTypes.h"
#include "SceneTypes.h"
#include "EntityComponentList.h"
#include <type_traits>
#include <tuple>

namespace Coco::ECS
{
	class Entity;
	class Scene;

	class ECSService : public EngineService
	{
	public:
		static constexpr uint64_t MaxEntities = 10000;

	private:
		static ECSService* _instance;

		Managed<MappedMemoryPool<Entity, MaxEntities>> _entities;
		Set<EntityID> _queuedEntitiesToDestroy;
		UnorderedMap<const char*, Managed<IEntityComponentList>> _componentLists;
		List<Managed<Scene>> _scenes;
		SceneID _nextSceneID = RootSceneID;

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
		bool IsDescendantOfEntity(EntityID entityID, EntityID parentID);

		void QueueDestroyEntity(EntityID entityID);
		void DestroyEntity(EntityID entityID);
		
		List<PackedSetData<Entity>>& GetEntities() { return _entities->GetSparseSet().Data(); }

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
		bool HasComponents(EntityID entityID) const
		{
			const char* type = typeid(ComponentType).name();

			auto* list = GetGenericComponentList(type);

			if (list == nullptr)
				return false;

			return list->HasComponent(entityID);
		}

		template<typename ComponentType, typename SecondComponentType, typename ... ComponentTypes>
		bool HasComponents(EntityID entityID) const
		{
			const char* type = typeid(ComponentType).name();

			auto* list = GetGenericComponentList(type);

			if (list == nullptr)
				return false;
			
			return list->HasComponent(entityID) && HasComponents<SecondComponentType, ComponentTypes...>(entityID);
		}

		template<typename ComponentType>
		bool RemoveComponent(EntityID entityID)
		{
			return GetComponentList<ComponentType>()->RemoveComponent(entityID);
		}

		Scene* CreateScene(const string& name = "", SceneID parentID = RootSceneID);
		Scene* GetRootScene() { return GetScene(RootSceneID); }
		Scene* GetScene(SceneID sceneID);
		bool TryGetScene(SceneID sceneID, Scene*& scene);
		bool IsDescendantOfScene(SceneID sceneID, SceneID parentID);
		bool IsEntityInScene(EntityID entityID, SceneID sceneID);
		void SetEntityScene(EntityID entityID, SceneID sceneID);
		void DestroyScene(SceneID sceneID);

		List<EntityID> GetSceneEntityIDs(SceneID scene) const;

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

		template<typename ComponentType>
		const EntityComponentList<ComponentType, MaxEntities>* GetComponentList() const
		{
			const char* key = typeid(ComponentType).name();

			if (!_componentLists.contains(key))
			{
				_componentLists.try_emplace(key, CreateManaged<EntityComponentList<ComponentType, MaxEntities>>());
			}

			return static_cast<EntityComponentList<ComponentType, MaxEntities>*>(_componentLists.at(key).get());
		}

		IEntityComponentList* GetGenericComponentList(const char* listKey)
		{
			if (!_componentLists.contains(listKey))
				return nullptr;

			return _componentLists.at(listKey).get();
		}

		const IEntityComponentList* GetGenericComponentList(const char* listKey) const
		{
			if (!_componentLists.contains(listKey))
				return nullptr;

			return _componentLists.at(listKey).get();
		}
	};
}