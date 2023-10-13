#pragma once
#include "../ComponentSerializer.h"

namespace Coco::ECS
{
    class EntityInfoComponentSerializer :
        public ComponentSerializer
    {
    protected:
        // Inherited via ComponentSerializer
        const char* GetSectionName() const override { return "EntityInfoComponent"; }
        void SerializeImpl(YAML::Emitter& emitter, const Entity& entity) override;
        void DeserializeImpl(const YAML::Node& baseNode, Entity& entity) override;
    };
}