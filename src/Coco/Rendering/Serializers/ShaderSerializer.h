#pragma once

#include <Coco/Core/Resources/ResourceSerializer.h>

namespace YAML
{
	class Emitter;
	class Node;
}

namespace Coco::Rendering
{
	struct ShaderVariant;

	/// @brief A ResourceSerializer for Shader resources
	class ShaderSerializer :
		public ResourceSerializer
	{
		// Inherited via ResourceSerializer
		bool SupportsFileExtension(const string& extension) const override;
		bool SupportsResourceType(const std::type_index& type) const override;
		string Serialize(SharedRef<Resource> resource) override;
		SharedRef<Resource> CreateAndDeserialize(const ResourceID& id, const string& data) override;
		bool Deserialize(const string& data, SharedRef<Resource> resource) override;
	};
}