#pragma once
#include "../SceneSystem.h"
#include <Coco/Core/Events/Event.h>

namespace Coco::ECS
{
    class Entity;

    /// @brief Updates any dirty transforms, taking into account parent-child relationships
    class TransformSystem :
        public SceneSystem
    {
    public:
        static const int sPriority;

    private:
        EventHandler<Entity&> _entityParentChangedHandler;

    public:
        TransformSystem(SharedRef<Scene> scene);
        ~TransformSystem();

        // Inherited via SceneSystem
        int GetPriority() const override { return sPriority; }
        void Tick() override;

        /// @brief Recalculates the transforms for the given entity and its children
        /// @param entity The entity
        /// @param preserveGlobalSpace If true, the entity will attempt to maintain its world transformation if it's parent changed
        static void RecalculateEntityTransforms(const Entity& entity, bool preserveGlobalSpace);
    };
}