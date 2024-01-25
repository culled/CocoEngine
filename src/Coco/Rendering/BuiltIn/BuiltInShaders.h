#pragma once

#include "../Shader.h"

namespace Coco::Rendering
{
	class BuiltInShaders
	{
	public:
		/// @brief Gets the built-in lit shader
		/// @return The lit shader
		static SharedRef<Shader> GetLitShader();

		/// @brief Gets the built-in unlit shader
		/// @return The unlit shader
		static SharedRef<Shader> GetUnlitShader();

	private:
		static ResourceID _litShaderID;
		static ResourceID _unlitShaderID;

	private:
		/// @brief Creates the lit shader
		/// @return The lit shader
		static SharedRef<Shader> CreateLitShader();

		/// @brief Creates the unlit shader
		/// @return The unlit shader
		static SharedRef<Shader> CreateUnlitShader();
	};
}