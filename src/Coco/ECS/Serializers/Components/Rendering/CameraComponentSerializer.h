#pragma once
#include "../../ComponentSerializer.h"
#include "../../../Components/Rendering/CameraComponent.h"

namespace Coco::ECS
{
    class CameraComponentSerializer :
        public ComponentSerializer<CameraComponent>
    {
    public:
        // Inherited via ComponentSerializer
        void Serialize(YAML::Emitter& emitter, const Entity& entity) override;
        bool Deserialize(const YAML::Node& baseNode, Entity& entity) override;
    };
}