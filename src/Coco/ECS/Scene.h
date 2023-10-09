#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
#include <Coco/Core/MainLoop/TickListener.h>
#include "entt.h"

namespace Coco::ECS
{
    class Entity;

    /// @brief A collection of Entities
    class Scene :
        public std::enable_shared_from_this<Scene>
    {
        friend class Entity;
        friend class SceneRender3DProvider;

    public:
        static const int sLateTickPriority;

    private:
        ManagedRef<TickListener> _lateTickListener;
        entt::registry _registry;
        std::vector<Entity> _queuedDestroyEntities;

    private:
        Scene();

    public:
        ~Scene();

        /// @brief Creates a Scene
        /// @return The created scene
        static SharedRef<Scene> Create();

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
        void HandleLateTick(const TickInfo& tickInfo);
    };
}