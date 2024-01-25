#pragma once
#include <Coco/Core/Events/Event.h>
#include "SceneSystem.h"

namespace Coco::ECS
{
    class Entity;

    /// @brief Updates any dirty transforms, taking into account parent-child relationships
    class TransformSystem :
        public SceneSystem
    {
    public:
        TransformSystem(Scene& scene);
        ~TransformSystem();

        // Inherited via SceneSystem
        int GetSetupPriority() const override { return 0; }
        void EntitiesAdded(std::span<Entity> rootEntities) override;

        /// @brief Recalculates the transforms for the given entity and its children
        /// @param entity The entity
        /// @param preserveGlobalSpace If true, the entity will attempt to maintain its world transformation if it's parent changed
        static void RecalculateEntityTransforms(Entity& entity);

    private:
        static EventHandler<Entity&> _entityParentChangedHandler;
    };
}