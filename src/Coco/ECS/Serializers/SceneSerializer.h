#pragma once
#include <Coco\Core\Resources\ResourceSerializer.h>
#include "ComponentSerializer.h"

namespace YAML
{
    class Emitter;
    class Node;
}

namespace Coco::ECS
{
    class Entity;
    class Scene;

    struct ComponentSerializerInstance
    {
        using TestFunctionType = std::function<bool(const Entity&)>;

        TestFunctionType TestFunc;
        UniqueRef<ComponentSerializer> Serializer;

        ComponentSerializerInstance(
            TestFunctionType testFunc, 
            UniqueRef<ComponentSerializer>&& serializer);
    };

    class SceneSerializer :
        public ResourceSerializer
    {
    private:
        static std::vector<ComponentSerializerInstance> _sComponentSerializers;

    public:
        SceneSerializer();
        ~SceneSerializer();

        template<typename ComponentType, typename SerializerType, typename ... Args>
        static void RegisterComponentSerializer(Args&& ... args)
        {
            _sComponentSerializers.emplace_back(
                [](const Entity& entity) { return entity.HasComponent<ComponentType>(); },
                CreateUniqueRef<SerializerType>(std::forward<Args>(args)...)
            );
        }

        // Inherited via ResourceSerializer
        bool SupportsFileExtension(const string& extension) const override;
        bool SupportsResourceType(const std::type_index& type) const override;
        const std::type_index GetResourceTypeForExtension(const string& extension) const override;
        string Serialize(SharedRef<Resource> resource) override;
        SharedRef<Resource> Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data) override;

    private:
        void SerializeEntity(YAML::Emitter& emitter, const Entity& entity);
        void DeserializeEntity(SharedRef<Scene>& scene, const YAML::Node& entityNode);

        void AddDefaultSerializers();
    };
}