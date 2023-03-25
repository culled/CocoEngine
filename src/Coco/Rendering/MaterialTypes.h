#pragma once

#include <Coco/Core/API.h>

namespace Coco::Rendering
{
	/// @brief A binding for a subshader to a material
	struct COCOAPI SubshaderUniformBinding
	{
		/// @brief The offset of the uniform (in bytes)
		uint64_t Offset;

		/// @brief The size of the binding (in bytes)
		uint64_t Size = 0;

		SubshaderUniformBinding(uint64_t offset) :
			Offset(offset), Size(0) {}
	};
}