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

    struct EntityComponent : public RTTIObject
    {
        DECLARE_RTTI_TYPE(EntityComponent)

    public:
        const UUID OwnerID;

        EntityComponent(const UUID& ownerID);

        Entity GetOwner();
    };
} // Coco

#endif //COCOENGINE_ENTITYCOMPONENT_H