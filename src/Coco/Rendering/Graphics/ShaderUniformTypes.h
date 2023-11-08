#pragma once

#include <Coco/Core/Types/String.h>
#include "ShaderTypes.h"

namespace Coco::Rendering
{
	/// @brief Scopes for a shader uniform
	enum class UniformScope
	{
		/// @brief Can only be updated once per render operation
		Global,

		/// @brief Can only be updated once per shader per render operation
		ShaderGlobal,

		/// @brief Can only be updated once per instance
		Instance,

		/// @brief Can be updated every draw call
		Draw
	};

	/// @brief Types of shader uniforms
	enum class ShaderUniformType
	{
		Float,
		Float2,
		Float3,
		Float4,
		Color,
		Mat4x4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Texture
	};

	enum class DefaultTextureType
	{
		White,
		FlatNormal,
		Checkered
	};

	/// @brief Gets the buffer data type for a data uniform
	/// @param dataUniform The type of data uniform
	/// @return The buffer data type of the data uniform
	BufferDataType GetBufferDataType(ShaderUniformType dataUniform);

	/// @brief Determines if the given uniform type is a data uniform (not a texture uniform)
	/// @return True if the given uniform type holds data
	bool IsDataShaderUniformType(ShaderUniformType type);

	/// @brief A uniform for a shader
	struct ShaderUniform
	{
		/// @brief The uniform name (used for referencing from materials)
		string Name;

		/// @brief The type of this uniform
		ShaderUniformType Type;

		/// @brief The points in the rendering stage when this uniform should be bound
		ShaderStageFlags BindingPoints;

		/// @brief The default value for this uniform
		std::any DefaultValue;

		/// @brief The auto-generated key from the name
		ShaderUniformData::UniformKey Key;

		ShaderUniform(const string& name, ShaderUniformType type, ShaderStageFlags bindPoints, std::any defaultValue);

		virtual ~ShaderUniform() = default;

		bool operator==(const ShaderUniform& other) const;
	};

	/// @brief A buffer uniform for a shader
	struct ShaderBufferUniform
	{
		/// @brief The uniform name (used for referencing from materials)
		string Name;

		/// @brief The points in the rendering stage when this uniform should be bound
		ShaderStageFlags BindingPoints;

		/// @brief The total size of the buffer
		uint64 Size;

		/// @brief The auto-generated key from the name
		ShaderUniformData::UniformKey Key;

		ShaderBufferUniform(const string& name, ShaderStageFlags bindPoints, uint64 size);

		bool operator==(const ShaderBufferUniform& other) const;
	};
}