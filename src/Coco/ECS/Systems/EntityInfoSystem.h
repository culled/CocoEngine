#pragma once
namespace Coco::ECS
{
    class Entity;

    class EntityInfoSystem
    {
    public:
        /// @brief Determines if the given entity is visible in its scene hierarchy
        /// @param entity The entity
        /// @return True if the entity is visible
        static bool IsEntityVisible(const Entity& entity);
    };
}