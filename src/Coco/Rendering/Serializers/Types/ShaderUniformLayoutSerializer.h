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
		/// @param outLayout The layout to deserialize into
		static void DeserializeLayout(const YAML::Node& baseNode, ShaderUniformLayout& outLayout);

		/// @brief Deserializes a GlobalShaderUniformLayout
		/// @param baseNode The base node
		/// @param outLayout The layout to deserialize into
		static void DeserializeGlobalLayout(const YAML::Node& baseNode, GlobalShaderUniformLayout& outLayout);
	};
}