#pragma once
#include <Coco\Core\Resources\ResourceSerializer.h>
#include "ComponentSerializer.h"

namespace Coco::ECS
{
    class Entity;
    class Scene;

    class SceneSerializer :
        public ResourceSerializer
    {
    public:
        SceneSerializer();
        ~SceneSerializer();

        // Inherited via ResourceSerializer
        bool SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource) override;
        SharedRef<Resource> CreateResource(const ResourceID& id) override;
        bool DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource) override;

    private:
        void SerializeEntity(YAML::Emitter& emitter, const Entity& entity);
        Entity DeserializeEntity(SharedRef<Scene>& scene, const string& entityName, const YAML::Node& entityNode, const Entity& parent);
    };
}