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
        string Serialize(Ref<Resource> resource) final;
        ManagedRef<Resource> Deserialize(const ResourceID& resourceID, const string& data) final;
    };
}