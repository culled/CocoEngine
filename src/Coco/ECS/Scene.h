#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/String.h>
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

    private:
        entt::registry _registry;

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

        void EachEntity(std::function<void(Entity&)> callback);
    };
}