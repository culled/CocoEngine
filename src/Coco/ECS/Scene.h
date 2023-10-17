#pragma once

#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "entt.h"
#include "EntityTypes.h"

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
        static const int sLateTickPriority;

    private:
        ManagedRef<TickListener> _lateTickListener;
        entt::registry _registry;
        std::vector<Entity> _queuedDestroyEntities;

    public:
        Scene(const ResourceID& id, const string& name);
        ~Scene();

        std::type_index GetType() const { return typeid(Scene); }

        /// @brief Creates an entity within this scene
        /// @param name The name of the entity
        /// @return The created entity
        Entity CreateEntity(const string& name = "Entity");

        /// @brief Queues an entity for destruction at the end of the frame
        /// @param entity The entity to destroy
        void DestroyEntity(Entity& entity);

        /// @brief Immediately destroys an entity
        /// @param entity The entity to destroy
        void DestroyEntityImmediate(Entity& entity);

        void EachEntity(std::function<void(Entity&)> callback);

    private:
        Entity CreateEntity(const string& name, const EntityID& id);

        void HandleLateTick(const TickInfo& tickInfo);
    };
}