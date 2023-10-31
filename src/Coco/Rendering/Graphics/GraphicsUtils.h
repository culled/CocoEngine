#pragma once

#include "ShaderTypes.h"
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_common.hpp>

namespace Coco::Rendering
{
	/// @brief Converts a ShaderStageType to a shaderc_shader_kind
	/// @param stage The stage type
	/// @return The equivalent shader kind
	shaderc_shader_kind ShaderStageToShaderC(ShaderStageType stage);

	/// @brief Converts a SPIRType to a BufferDataType
	/// @param type The data type
	/// @return The converted buffer data type
	BufferDataType SPIRTypeToBufferDataType(const spirv_cross::SPIRType& type);
}