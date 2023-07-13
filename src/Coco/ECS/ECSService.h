#pragma once

#include <Coco/Core/Services/EngineService.h>
#include <Coco/Core/Types/Set.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/Singleton.h>
#include "Entity.h"
#include "Scene.h"
#include "EntityComponentList.h"
#include <type_traits>
#include <tuple>

namespace Coco::ECS
{
	class ECSService : public EngineService, public Singleton<ECSService>
	{
	private:
		UnorderedMap<EntityID, Entity> _entities;
		Set<EntityID> _queuedEntitiesToDestroy;
		UnorderedMap<std::type_index, ManagedRef<IEntityComponentList>> _componentLists;
		List<ManagedRef<Scene>> _scenes;
		SceneID _nextSceneID = RootSceneID;

	public:
		/// @brief Priority for the tick handling entities
		const static int ProcessTickPriority = -20000;

		ECSService();
		~ECSService();

		EntityID CreateEntity(const string& name = "", const EntityID& parentID = InvalidEntityID);

		Entity& GetEntity(const EntityID& entityID);
		bool TryGetEntity(const EntityID& entityID, Entity*& entity);

		List<EntityID> GetEntityChildrenIDs(const EntityID& entity);
		List<Entity*> GetEntityChildren(const EntityID& entity);

		Entity& GetEntityParent(const EntityID& entityID);
		bool TryGetEntityParent(const EntityID& entityID, Entity*& entity);
		bool IsDescendantOfEntity(const EntityID& entityID, const EntityID& parentID);

		void QueueDestroyEntity(const EntityID& entityID);
		void DestroyEntity(const EntityID& entityID);
		
		List<Entity*> GetEntities();

		template<typename ComponentType, typename ... Args>
		ComponentType& AddComponent(const EntityID& entityID, Args&& ... args)
		{
			return GetComponentList<ComponentType>()->Add(entityID, std::forward<Args>(args)...);
		}

		template<typename ComponentType>
		ComponentType& GetComponent(const EntityID& entityID)
		{
			// Check for exact type match
			if (_componentLists.contains(typeid(ComponentType)))
				return *static_cast<ComponentType*>(&(_componentLists.at(typeid(ComponentType))->Get(entityID)));

			// Check for runtime-derived types
			for (auto& kvp : _componentLists)
			{
				if (kvp.second->Has(entityID))
				{
					ComponentType* component = dynamic_cast<ComponentType*>(&(kvp.second->Get(entityID)));

					if (component != nullptr)
						return *component;
				}
			}

			throw Exception(FormattedString("Entity {} has no component of type {}", entityID.AsString(), typeid(ComponentType).name()));
		}

		template<typename ComponentType>
		bool HasComponent(const EntityID& entityID) const
		{
			return HasComponents<ComponentType>(entityID);
		}

		template<typename ComponentType>
		bool HasComponents(const EntityID& entityID) const
		{
			// Check for exact type match
			if (_componentLists.contains(typeid(ComponentType)))
				return _componentLists.at(typeid(ComponentType))->Has(entityID);

			// Check for runtime-derived types
			for (const auto& kvp : _componentLists)
			{
				if (kvp.second->Has(entityID) && dynamic_cast<const ComponentType*>(&(kvp.second->Get(entityID))))
				{
					return true;
				}
			}

			// No match
			return false;
		}

		template<typename ComponentType, typename SecondComponentType, typename ... ComponentTypes>
		bool HasComponents(const EntityID& entityID) const
		{
			// Check for exact type match
			if(_componentLists.contains(typeid(ComponentType)))
				return _componentLists.at(typeid(ComponentType))->Has(entityID) && HasComponents<SecondComponentType, ComponentTypes...>(entityID);

			// Check for runtime-derived types
			for (const auto& kvp : _componentLists)
			{
				if (kvp.second->Has(entityID) && dynamic_cast<const ComponentType*>(&(kvp.second->Get(entityID))))
				{
					return HasComponents<SecondComponentType, ComponentTypes...>(entityID);
				}
			}
			
			// No match
			return false;
		}

		template<typename ComponentType>
		bool RemoveComponent(const EntityID& entityID)
		{
			return GetComponentList<ComponentType>()->Remove(entityID);
		}

		Scene* CreateScene(const string& name = "", SceneID parentID = RootSceneID);
		Scene* GetRootScene() { return GetScene(RootSceneID); }
		Scene* GetScene(SceneID sceneID);
		bool TryGetScene(SceneID sceneID, Scene*& scene);
		bool IsDescendantOfScene(SceneID sceneID, SceneID parentID);
		bool IsEntityInScene(const EntityID& entityID, SceneID sceneID);
		void SetEntityScene(const EntityID& entityID, SceneID sceneID);
		void DestroyScene(SceneID sceneID);

		List<EntityID> GetSceneEntityIDs(SceneID scene) const;

	private:
		/// @brief Tick for updating any entities
		/// @param deltaTime The number of seconds since the last tick
		void Process(double deltaTime);

		EntityID GetNextEntityID();

		template<typename ComponentType>
		EntityComponentList<ComponentType>* GetComponentList()
		{
			std::type_index key = typeid(ComponentType);

			if (!_componentLists.contains(key))
			{
				_componentLists.try_emplace(key, CreateManagedRef<EntityComponentList<ComponentType>>());
			}

			return static_cast<EntityComponentList<ComponentType>*>(_componentLists.at(key).Get());
		}

		template<typename ComponentType>
		const EntityComponentList<ComponentType>* GetComponentList() const
		{
			std::type_index key = typeid(ComponentType);

			if (!_componentLists.contains(key))
			{
				return nullptr;
			}

			return static_cast<const EntityComponentList<ComponentType>*>(_componentLists.at(key).Get());
		}
	};
}