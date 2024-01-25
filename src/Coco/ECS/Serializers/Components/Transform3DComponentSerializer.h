#pragma once
#include "../ComponentSerializer.h"
#include "../../Components/Transform3DComponent.h"

namespace Coco::ECS
{
    class Transform3DComponentSerializer :
        public ComponentSerializer<Transform3DComponent>
    {
    public:
        // Inherited via ComponentSerializer
        void Serialize(YAML::Emitter& emitter, const Entity& entity) override;
        bool Deserialize(const YAML::Node& baseNode, Entity& entity) override;
    };
}