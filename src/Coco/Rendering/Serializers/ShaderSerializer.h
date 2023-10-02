#pragma once

#include <Coco/Core/Resources/ResourceSerializer.h>

namespace YAML
{
	class Emitter;
	class Node;
}

namespace Coco::Rendering
{
	struct RenderPassShader;

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
		/// @brief Serializes a RenderPassShader
		/// @param emitter The emitter
		/// @param passShader The pass shader
		void SerializeRenderPassShader(YAML::Emitter& emitter, const RenderPassShader& passShader);

		/// @brief Deserializes a RenderPassShader
		/// @param baseNode The base node of the render pass shader
		/// @return The deserialized pass shader
		RenderPassShader DeserializeRenderPassShader(const YAML::Node& baseNode);
	};
}