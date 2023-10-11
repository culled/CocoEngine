#pragma once
#include <Coco\Core\Resources\ResourceSerializer.h>

namespace Coco::Rendering
{
    class MeshSerializer :
        public ResourceSerializer
    {
		// Inherited via ResourceSerializer
		bool SupportsFileExtension(const string& extension) const override;
		bool SupportsResourceType(const std::type_index& type) const override;
		const std::type_index GetResourceTypeForExtension(const string& extension) const final;
		string Serialize(SharedRef<Resource> resource) override;
		SharedRef<Resource> Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data) override;
    };
}