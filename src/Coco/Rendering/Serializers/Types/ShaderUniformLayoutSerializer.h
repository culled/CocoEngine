#pragma once
#include "../../Graphics/ShaderUniformLayout.h"

namespace YAML
{
	class Emitter;
	class Node;
}

namespace Coco::Rendering
{
	/// @brief Provides serialization for ShaderUniformLayouts
	struct ShaderUniformLayoutSerialization
	{
		/// @brief Serializes a ShaderUniformLayout
		/// @param emitter The emitter
		/// @param layout The layout
		static void SerializeLayout(YAML::Emitter& emitter, const ShaderUniformLayout& layout);

		/// @brief Serializes a GlobalShaderUniformLayout
		/// @param emitter The emitter
		/// @param layout The layout
		static void SerializeGlobalLayout(YAML::Emitter& emitter, const GlobalShaderUniformLayout& layout);

		/// @brief Deserializes a ShaderUniformLayout
		/// @param baseNode The base node
		/// @return The deserialized layout
		static ShaderUniformLayout DeserializeLayout(const YAML::Node& baseNode);

		/// @brief Deserializes a GlobalShaderUniformLayout
		/// @param baseNode The base node
		/// @return The deserialized layout
		static GlobalShaderUniformLayout DeserializeGlobalLayout(const YAML::Node& baseNode);

		/// @brief Serializes the data for a ShaderUniform
		/// @param emitter The emitter
		/// @param uniform The uniform
		static void SerializeBaseUniform(YAML::Emitter& emitter, const ShaderUniform& uniform);

		/// @brief Deserializes the data for a ShaderUniformLayout
		/// @param baseNode The base node
		/// @param outDataUniforms Will be filled with the data uniforms
		/// @param outTextureUniforms Will be filled with the texture uniforms
		static void DeserializeBaseUniformData(
			const YAML::Node& baseNode, 
			std::vector<ShaderDataUniform>& outDataUniforms, 
			std::vector<ShaderTextureUniform>& outTextureUniforms);
	};
}