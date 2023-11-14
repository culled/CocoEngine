#pragma once

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include <Coco/Core/Events/Event.h>
#include "entt.h"
#include "EntityTypes.h"
#include "SceneSystem.h"

namespace Coco::ECS
{
    class Entity;

    /// @brief A collection of Entities
    class Scene :
        public Resource
    {
        friend class Entity;
        friend class SceneSerializer;

        template<typename, typename...>
        friend class SceneView;

    public:
        static const int sTickPriority;
        static const int sLateTickPriority;

        /// @brief Invoked when an entity's parent has changed
        Event<Entity&> OnEntityParentChanged;

    private:
        ManagedRef<TickListener> _tickListener;
        ManagedRef<TickListener> _lateTickListener;
        entt::registry _registry;
        std::vector<Entity> _queuedDestroyEntities;
        std::unordered_map<EntityID, EntityID> _entityParentMap;
        std::vector<UniqueRef<SceneSystem>> _systems;
        bool _systemsNeedSorting;

    public:
        Scene(const ResourceID& id, const string& name);
        ~Scene();

        std::type_index GetType() const { return typeid(Scene); }

        /// @brief Creates a scene with default systems
        /// @param name The name of the scene
        /// @return The created scene
        static SharedRef<Scene> CreateWithDefaultSystems(const string& name);

        /// @brief Creates an entity within this scene
        /// @param name The name of the entity
        /// @return The created entity
        Entity CreateEntity(const string& name = "Entity");

        /// @brief Gets the entity associated with the given ID
        /// @param id The entity ID
        /// @param outEntity Will be set to the entity if found
        /// @return True if an entity with the given ID was found in this scene
        bool TryGetEntity(const EntityID& id, Entity& outEntity);

        /// @brief Sets the parent of an entity
        /// @param entityID The entity ID
        /// @param parentID The ID of the new parent entity, or InvalidEntityID to clear the parent
        void ReparentEntity(const EntityID& entityID, const EntityID& parentID);

        /// @brief Gets all un-parented entities in this scene
        /// @return Unparented entities
        std::vector<Entity> GetRootEntities();

        /// @brief Queues an entity for destruction at the end of the frame
        /// @param entity The entity to destroy
        void DestroyEntity(Entity& entity);

        /// @brief Immediately destroys an entity
        /// @param entity The entity to destroy
        void DestroyEntityImmediate(Entity& entity);

        /// @brief Performs a callback for each entity in this scene
        /// @param callback The callback to call
        void EachEntity(std::function<void(Entity&)> callback);

        /// @brief Causes this scene to use a system that automatically performs updates of components during the late tick
        /// @tparam SystemType The type of system
        /// @tparam ...Args The type of constructor arguments
        /// @param ...args The arguments to pass to the system's constructor
        template<typename SystemType, typename ... Args>
        void UseSystem(Args&& ... args)
        {
            _systems.push_back(CreateUniqueRef<SystemType>(GetSelfRef<Scene>(), std::forward<Args>(args)...));
            _systemsNeedSorting = true;
        }

        /// @brief Attempts to find the entity that a component instance is attached to
        /// @tparam ComponentType The type of component
        /// @param component The component instance
        /// @param outEntity Will be set to the component's entity if found
        /// @return True if the component's entity was found
        template<typename ComponentType>
        bool TryGetComponentEntity(const ComponentType& component, Entity& outEntity)
        {
            auto entity = entt::to_entity(_registry, component);

            if (entity == entt::null)
            {
                outEntity = Entity::Null;
                return false;
            }

            outEntity = Entity(entity, GetSelfRef<Scene>());
            return true;
        }

        /// @brief Causes default systems to be used
        void UseDefaultSystems();

    private:
        /// @brief Sorts all systems by their priorities
        void SortSystems();

        /// @brief Creates an entity with a name and ID
        /// @param name The entity's name
        /// @param id The entity's ID
        /// @param parent The parent for this entity
        /// @return The entity
        Entity CreateEntity(const string& name, const EntityID& id, const Entity& parent);

        /// @brief Handler for the tick
        /// @param tickInfo The tick info
        void HandleTick(const TickInfo& tickInfo);

        /// @brief Handler for the late tick
        /// @param tickInfo The tick info
        void HandleLateTick(const TickInfo& tickInfo);

        /// @brief Clears all entities from this scene
        void Clear();
    };
}