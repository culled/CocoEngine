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
		const std::type_index GetResourceTypeForExtension(const string& extension) const final;
		string Serialize(Ref<Resource> resource) override;
		ManagedRef<Resource> Deserialize(const std::type_index& type, const ResourceID& resourceID, const string& data) override;

	private:
		/// @brief Serializes a ShaderVariant
		/// @param emitter The emitter
		/// @param variant The shader variant
		void SerializeShaderVariant(YAML::Emitter& emitter, const ShaderVariant& variant);

		/// @brief Deserializes a ShaderVariant
		/// @param baseNode The base node of the render pass shader
		/// @return The deserialized shader variant
		ShaderVariant DeserializeShaderVariant(const YAML::Node& baseNode);
	};
}