#pragma once
#include <Coco\Core\Resources\ResourceSerializer.h>

namespace Coco::Rendering
{
    /// @brief A ResourceSerializer for Texture resources
    class TextureSerializer :
        public ResourceSerializer
    {
        // Inherited via ResourceSerializer
        bool SupportsFileExtension(const string& extension) const final;
        bool SupportsResourceType(const std::type_index& type) const final;
        string Serialize(SharedRef<Resource> resource) final;
        SharedRef<Resource> CreateAndDeserialize(const ResourceID& id, const string& data) final;
        bool Deserialize(const string& data, SharedRef<Resource> resource) final;
    };
}