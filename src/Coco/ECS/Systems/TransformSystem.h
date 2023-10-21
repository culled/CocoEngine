#pragma once
#include "../SceneSystem.h"

namespace Coco::ECS
{
    class Entity;

    struct Transform3DComponent;

    /// @brief Updates any dirty transforms, taking into account parent-child relationships
    class TransformSystem :
        public SceneSystem
    {
    public:
        static const int sPriority;

    public:
        // Inherited via SceneSystem
        int GetPriority() const override { return sPriority; }
        void Execute(SharedRef<Scene> scene) override;

        /// @brief Recalculates the 3D transform for a given entity
        /// @param entity The entity
        /// @param forceUpdate If true, the transform will be recalculated regardless if it is marked dirty
        static void UpdateTransform3D(Entity& entity, bool forceUpdate = false);

        /// @brief Marks the entity and all its children as needing to be updated
        /// @param entity The entity to mark dirty
        static void MarkTransform3DDirty(Entity& entity);

        /// @brief Attempts to get the Transform3DComponent of the given entity, if it exists
        /// @param entity The entity
        /// @param outTransform Will be set to the parent's transform, if found
        /// @return True if the entity has a parent with a Transform3DComponent
        static bool TryGetParentTransform3D(Entity& entity, Transform3DComponent*& outTransform);

    private:
        /// @brief Updates the 3D transforms for a given scene
        /// @param scene The scene
        static void Update3DTransforms(SharedRef<Scene>& scene);
    };
}