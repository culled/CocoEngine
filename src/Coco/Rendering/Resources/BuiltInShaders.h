#pragma once
#include "../Shader.h"

namespace Coco::Rendering
{
	/// @brief Built-in shader variants
	class BuiltInShaders
	{
	private:
		static ResourceID _litShaderID;
		static ResourceID _unlitShaderID;

	public:
		/// @brief Gets the built-in lit shader
		/// @return The lit shader
		static SharedRef<Shader> GetLitShader();

		/// @brief Gets the built-in unlit shader
		/// @return The unlit shader
		static SharedRef<Shader> GetUnlitShader();

	private:
		/// @brief Creates the lit shader
		/// @return The lit shader
		static SharedRef<Shader> CreateLitShader();

		/// @brief Creates the unlit shader
		/// @return The unlit shader
		static SharedRef<Shader> CreateUnlitShader();
	};
}