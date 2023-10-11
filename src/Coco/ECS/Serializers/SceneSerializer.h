#pragma once
#include <Coco\Core\Resources\ResourceSerializer.h>

namespace YAML
{
    class Emitter;
    class Node;
}

namespace Coco::ECS
{
    class Entity;
    class Scene;

    class SceneSerializer :
        public ResourceSerializer
    {
    public:
        // Inherited via ResourceSerializer
        bool SupportsFileExtension(const string& extension) const override;
        bool SupportsResourceType(const std::type_index& type) const override;
        const std::type_index GetResourceTypeForExtension(const string& extension) const override;
        string Serialize(SharedRef<Resource> resource) override;
        SharedRef<Resource> Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data) override;

    private:
        void SerializeEntity(YAML::Emitter& emitter, const Entity& entity);
        void DeserializeEntity(SharedRef<Scene>& scene, const YAML::Node& entityNode);
    };
}