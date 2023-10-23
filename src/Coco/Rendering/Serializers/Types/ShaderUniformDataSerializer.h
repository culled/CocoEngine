#pragma once

#include "../../Graphics/ShaderUniformData.h"
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
	};
}