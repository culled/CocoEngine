#pragma once
#include "../Shader.h"

namespace Coco::Rendering
{
	/// @brief Built-in shader variants
	struct BuiltInShaders
	{
		/// @brief The built-in lit shader variant
		static const ShaderVariant LitVariant;

		/// @brief The built-in unlit shader variant
		static const ShaderVariant UnlitVariant;
	};
}