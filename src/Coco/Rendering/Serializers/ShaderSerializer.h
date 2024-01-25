#pragma once
#include <Coco/Core/Resources/ResourceSerializer.h>

namespace Coco::Rendering
{
    class ShaderUniformLayout;

    class ShaderSerializer :
        public ResourceSerializer
    {
    public:

        // Inherited via ResourceSerializer
        bool SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource) override;
        SharedRef<Resource> CreateResource(const ResourceID& id) override;
        bool DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource) override;

    private:
        void SerializeUniformLayout(YAML::Emitter& emitter, const ShaderUniformLayout& layout);
        ShaderUniformLayout DeserializeUniformLayout(const YAML::Node& baseNode);
    };
}