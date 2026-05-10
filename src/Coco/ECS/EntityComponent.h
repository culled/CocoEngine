//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_ENTITYCOMPONENT_H
#define COCOENGINE_ENTITYCOMPONENT_H
#include "Coco/Core/Types/UUID.h"
#include "Coco/Core/RTTI/RTTI.h"

namespace Coco
{
    class Entity;

    /// @brief Base class for all components that can be attached to Entities
    struct EntityComponent : public RTTIObject
    {
        DECLARE_RTTI_TYPE(EntityComponent)

    public:
        /// @brief The ID of the Entity that this component is attached to
        const UUID OwnerID;

        EntityComponent(const UUID& ownerID);

        /// @brief Gets the Entity that this component is attached to
        /// @return The Entity that this component is attached to
        Entity GetOwner();
    };
} // Coco

#endif //COCOENGINE_ENTITYCOMPONENT_H