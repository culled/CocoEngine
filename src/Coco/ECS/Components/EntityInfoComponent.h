#pragma once
#include <Coco/Core/Types/String.h>
#include "../EntityTypes.h"

namespace Coco::ECS
{
    /// @brief A component that stores vital information for an entity
    struct EntityInfoComponent
    {
        /// @brief The name of the entity
        string Name;

        /// @brief The ID of the entity
        EntityID ID;

        /// @brief If true, this entity and its children are active in the scene
        bool IsActive;

        EntityInfoComponent();
        EntityInfoComponent(const string& name, const EntityID& id);
    };
}