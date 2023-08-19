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
	/// @brief A service that manages entities and components
	class ECSService : public EngineService, public Singleton<ECSService>
	{
		friend class ManagedRef<ECSService>;

	private:
		UnorderedMap<EntityID, Entity> _entities;
		Set<EntityID> _queuedEntitiesToDestroy;
		UnorderedMap<std::type_index, ManagedRef<IEntityComponentList>> _componentLists;
		List<ManagedRef<Scene>> _scenes;
		SceneID _nextSceneID = RootSceneID;

	public:
		/// @brief Priority for the tick handling entities
		const static int ProcessTickPriority = -20000;

	protected:
		ECSService();

	public:
		~ECSService();

		/// @brief Creates a new entity
		/// @param name The name of the entity
		/// @param parentID If given, will be the parent of the entity
		/// @return The ID of the created entity
		EntityID CreateEntity(const string& name = "", const EntityID& parentID = InvalidEntityID);

		/// @brief Gets an entity by its ID
		/// @param entityID The id of the entity
		/// @return The entity
		Entity& GetEntity(const EntityID& entityID);

		/// @brief Gets an entity by its ID, checking if it exists first
		/// @param entityID The ID of the entity
		/// @param entity Will be set to the entity if it exists
		/// @return True if the entity exists
		bool TryGetEntity(const EntityID& entityID, Entity*& entity);

		/// @brief Gets the IDs of an entity's children
		/// @param entity The entity ID
		/// @return A list of IDs of the entity's children
		List<EntityID> GetEntityChildrenIDs(const EntityID& entity) const;

		/// @brief Gets the child entities of an entity
		/// @param entity The entity ID
		/// @return A list of child entities
		List<Entity*> GetEntityChildren(const EntityID& entity);

		/// @brief Gets the parent of an entity
		/// @param entityID The ID of the entity
		/// @return The parent entity
		Entity& GetEntityParent(const EntityID& entityID);

		/// @brief Gets the parent of an entity, checking if it exists first
		/// @param entityID The ID of the entity
		/// @param entity Will be set to the entity's parent if it exists
		/// @return True if the entity's parent exists
		bool TryGetEntityParent(const EntityID& entityID, Entity*& entity);

		/// @brief Determines if an entity is a descendant of an entity
		/// @param entityID The ID of the entity
		/// @param ancestorID The ID of the ancestor entity
		/// @return True if the given entity is a child of the ancestor entity
		bool IsDescendantOfEntity(const EntityID& entityID, const EntityID& ancestorID);

		/// @brief Queues an entity for destruction
		/// @param entityID The ID of the entity
		void QueueDestroyEntity(const EntityID& entityID);

		/// @brief Immediately destroys an entity
		/// @param entityID The ID of the entity
		void DestroyEntity(const EntityID& entityID);
		
		/// @brief Gets all entities
		/// @return A list of all entities
		List<Entity*> GetEntities();

		/// @brief Creates a component and attaches it to an entity
		/// @tparam ComponentType The type of component to create
		/// @tparam ...Args The argument types
		/// @param entityID The entity ID
		/// @param ...args Arguments to pass to the component's constructor
		/// @return The created component
		template<typename ComponentType, typename ... Args>
		ComponentType& AddComponent(const EntityID& entityID, Args&& ... args)
		{
			return GetOrCreateComponentList<ComponentType>()->Add(entityID, std::forward<Args>(args)...);
		}

		/// @brief Gets a component on an entity
		/// @tparam ComponentType The type of component
		/// @param entityID The entity ID
		/// @return The entity's component
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

		/// @brief Checks if an entity has a component
		/// @tparam ComponentType The type of component
		/// @param entityID The entity ID
		/// @return True if the entity has a component of the matching type
		template<typename ComponentType>
		bool HasComponent(const EntityID& entityID) const
		{
			return HasComponents<ComponentType>(entityID);
		}

		/// @brief Checks if an entity has all of the component types
		/// @tparam ComponentType The type of component
		/// @param entityID The entity ID
		/// @return True if the entity has components of the matching types
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

		/// @brief Checks if an entity has all of the component types
		/// @tparam ComponentType The type of component
		/// @tparam SecondComponentType The type of component
		/// @tparam ComponentTypes The types of components
		/// @param entityID The entity ID
		/// @return True if the entity has components of the matching types
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

		/// @brief Removes a component from the given entity
		/// @tparam ComponentType The type of component
		/// @param entityID The entity ID
		/// @return True if the entity had the component type and it was removed
		template<typename ComponentType>
		bool RemoveComponent(const EntityID& entityID)
		{
			return GetOrCreateComponentList<ComponentType>()->Remove(entityID);
		}

		/// @brief Creates a scene
		/// @param name The name of the scene
		/// @param parentID If given, will be the parent of this scene
		/// @return The created scene
		Scene* CreateScene(const string& name = "", const SceneID& parentID = RootSceneID);

		/// @brief Gets the root scene
		/// @return The root scene
		Scene* GetRootScene() { return GetScene(RootSceneID); }

		/// @brief Gets a scene by its ID
		/// @param sceneID The ID of the scene
		/// @return The scene
		Scene* GetScene(const SceneID& sceneID);

		/// @brief Gets a scene by its ID, checking first if it exists
		/// @param sceneID The ID of the scene
		/// @param scene Will be set to the scene if it exists
		/// @return True if the scene exists
		bool TryGetScene(const SceneID& sceneID, Scene*& scene);

		/// @brief Determines if the given scene is a descendant of an ancestor scene
		/// @param sceneID The scene ID
		/// @param ancestorID The ancestor scene ID
		/// @return True if the scene is a descendant of the ancestor scene
		bool IsDescendantOfScene(const SceneID& sceneID, const SceneID& ancestorID);

		/// @brief Determines if an entity is a part of a scene
		/// @param entityID The entity ID
		/// @param sceneID The scene ID
		/// @return True if the given entity exists in the given scene
		bool IsEntityInScene(const EntityID& entityID, const SceneID& sceneID);

		/// @brief Sets the scene that an entity exists in
		/// @param entityID The entity ID
		/// @param sceneID The scene ID
		void SetEntityScene(const EntityID& entityID, const SceneID& sceneID);

		/// @brief Destroys a scene
		/// @param sceneID The scene
		void DestroyScene(const SceneID& sceneID);

		/// @brief Gets the IDs of entities that exist in a given scene
		/// @param sceneID The scene ID
		/// @return A list of entity IDs that exist in the scene
		List<EntityID> GetSceneEntityIDs(const SceneID& sceneID) const;

	private:
		/// @brief Tick for updating any entities
		/// @param deltaTime The number of seconds since the last tick
		void Process(double deltaTime);

		/// @brief Gets a new entity ID
		/// @return An entity ID
		EntityID GetNextEntityID();

		/// @brief Gets the component list for a component type, or creates it if one does not exist
		/// @tparam ComponentType The type of component
		/// @return The component list
		template<typename ComponentType>
		EntityComponentList<ComponentType>* GetOrCreateComponentList()
		{
			std::type_index key = typeid(ComponentType);

			if (!_componentLists.contains(key))
			{
				_componentLists.try_emplace(key, CreateManagedRef<EntityComponentList<ComponentType>>());
			}

			return static_cast<EntityComponentList<ComponentType>*>(_componentLists.at(key).Get());
		}

		/// @brief Gets the component list for a component type
		/// @tparam ComponentType The type of component
		/// @return The component list
		template<typename ComponentType>
		const EntityComponentList<ComponentType>* GetOrCreateComponentList() const
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