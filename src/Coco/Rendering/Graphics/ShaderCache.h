#pragma once

#include <Coco/Core/Types/String.h>
#include "../Shader.h"

namespace Coco::Rendering
{
	/// @brief A cache that can be used to load Shaders
	class ShaderCache
	{
	public:
		virtual ~ShaderCache() = default;

		//virtual Shader LoadShader(const std::unordered_map<ShaderStageType, string>& stageFiles) = 0;
	};
}