#pragma once

#include <Coco/Core/Types/String.h>
#include "ShaderVariant.h"

namespace Coco::Rendering
{
	/// @brief A cache that can be used to load ShaderVariants 
	class ShaderCache
	{
	public:
		virtual ~ShaderCache() = default;

		/// @brief Creates a ShaderVariant from a combination of source files
		/// @param stageFiles The files for each shader stage
		/// @return The created shader variant
		virtual ShaderVariant LoadVariant(const std::unordered_map<ShaderStageType, string>& stageFiles) = 0;
	};
}