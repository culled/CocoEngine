#pragma once
#include "../ComponentSerializer.h"
#include "../../Components/EntityInfoComponent.h"

namespace Coco::ECS
{
    class EntityInfoComponentSerializer :
        public ComponentSerializer<EntityInfoComponent>
    {
    public:
        // Inherited via ComponentSerializer
        void Serialize(YAML::Emitter& emitter, const Entity& entity) override;
        bool Deserialize(const YAML::Node& baseNode, Entity& entity) override;
    };
}