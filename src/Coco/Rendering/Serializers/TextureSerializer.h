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
        const std::type_index GetResourceTypeForExtension(const string& extension) const final;
        string Serialize(SharedRef<Resource> resource) final;
        SharedRef<Resource> Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data) final;
    };
}