//
// Created by cullen on 4/1/26.
//

#include "EntityComponent.h"
#include "Entity.h"
#include "ECSService.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    DEFINE_RTTI_BASETYPE(EntityComponent);

    EntityComponent::EntityComponent(const UUID& ownerID) :
        OwnerID(ownerID)
    {}

    Entity EntityComponent::GetOwner()
    {
        if (auto engine = Engine::Get())
        {
            if (auto ecs = engine->GetService<ECSService>())
            {
                return ecs->GetEntity(OwnerID);
            }
        }

        return Entity();
    }
} // Coco