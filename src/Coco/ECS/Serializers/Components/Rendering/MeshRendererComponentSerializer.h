#pragma once
#include "../../ComponentSerializer.h"
#include "../../../Components/Rendering/MeshRendererComponent.h"

namespace Coco::ECS
{
    class MeshRendererComponentSerializer :
        public ComponentSerializer<MeshRendererComponent>
    {
    public:
        // Inherited via ComponentSerializer
        void Serialize(YAML::Emitter& emitter, const Entity& entity) override;
        bool Deserialize(const YAML::Node& baseNode, Entity& entity) override;
    };
}