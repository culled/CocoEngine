#pragma once
#include <Coco/Core/Types/String.h>
#include "../EntityTypes.h"
#include "../Entity.h"

namespace Coco::ECS
{
    /// @brief A component that stores vital information for an entity
    struct EntityInfoComponent
    {
        /// @brief The name of the entity
        string Name;

        /// @brief The ID of the entity
        uint64 ID;

        /// @brief The parent of the entity, if any
        std::optional<Entity> Parent;

        /// @brief If true, this entity and its children are active in the scene
        bool IsActive;

        EntityInfoComponent();
        EntityInfoComponent(const string& name, const EntityID& id, std::optional<Entity> parent);
    };
}