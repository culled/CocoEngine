#pragma once
#include <Coco/Core/Resources/ResourceSerializer.h>

namespace Coco::Rendering
{
    class TextureSerializer :
        public ResourceSerializer
    {
    public:

        // Inherited via ResourceSerializer
        bool SerializeYAML(YAML::Emitter& emitter, const SharedRef<Resource>& resource) override;
        SharedRef<Resource> CreateResource(const ResourceID& id) override;
        bool DeserializeYAML(const YAML::Node& baseNode, SharedRef<Resource> resource) override;
    };
}