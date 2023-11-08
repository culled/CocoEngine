#pragma once

#include "../../Graphics/ShaderUniformData.h"
#include "../../Graphics/ShaderUniformTypes.h"
#include "../../Texture.h"

namespace YAML
{
	class Emitter;
	class Node;
}

namespace Coco::Rendering
{
	/// @brief Provides serialization for ShaderUniformData
	struct ShaderUniformDataSerializer
	{
		/// @brief Serializes a ShaderUniformData and its associated textures
		/// @param emitter The emitter
		/// @param data The uniform data
		/// @param textures The textures
		static void Serialize(YAML::Emitter& emitter, const ShaderUniformData& data, const std::unordered_map<ShaderUniformData::UniformKey, SharedRef<Texture>>& textures);

		/// @brief Deserializes a ShaderUniformData and its associated textures
		/// @param baseNode The base node
		/// @param outData Will be filled out with the uniform data
		/// @param outTextures Will be filled out with the texture data
		static void Deserialize(const YAML::Node& baseNode,  ShaderUniformData& outData, std::unordered_map<ShaderUniformData::UniformKey, SharedRef<Texture>>& outTextures);

		/// @brief Serializes a ShaderUniformUnion
		/// @param emitter The emitter
		/// @param uniformUnion The union
		static void SerializeUniformUnion(YAML::Emitter& emitter, const ShaderUniformUnion& uniformUnion);

		/// @brief Deserializes a ShaderUniformUnion
		/// @param baseNode The base node
		/// @param outUniformUnion Will be filled out with the union data
		static void DeserializeUniformUnion(const YAML::Node& baseNode, ShaderUniformUnion& outUniformUnion);
	};
}