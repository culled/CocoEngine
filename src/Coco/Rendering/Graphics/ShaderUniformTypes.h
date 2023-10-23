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

	/// @brief A uniform for a shader
	struct ShaderUniform
	{
		/// @brief The uniform name (used for referencing from materials)
		string Name;

		/// @brief The points in the rendering stage when this uniform should be bound
		ShaderStageFlags BindingPoints;

		/// @brief The auto-generated key from the name
		ShaderUniformData::UniformKey Key;

		ShaderUniform(const string& name, ShaderStageFlags bindPoints);

		virtual ~ShaderUniform() = default;

		bool operator==(const ShaderUniform& other) const;
	};

	/// @brief A texture uniform for a shader
	struct ShaderTextureUniform : 
		public ShaderUniform
	{
		/// @brief Types of default textures
		enum class DefaultTextureType
		{
			White,
			Checker,
			Normal
		};

		/// @brief The default for this texture
		DefaultTextureType DefaultTexture;

		ShaderTextureUniform(const string& name, ShaderStageFlags bindPoints, DefaultTextureType defaultTexture);

		bool operator==(const ShaderTextureUniform& other) const;
	};

	/// @brief A data uniform for a shader
	struct ShaderDataUniform : 
		public ShaderUniform
	{
		/// @brief The type of data
		BufferDataType Type;

		ShaderDataUniform(const string& name, ShaderStageFlags bindPoints, BufferDataType type);

		bool operator==(const ShaderDataUniform& other) const;
	};

	/// @brief A buffer uniform for a shader
	struct ShaderBufferUniform :
		public ShaderUniform
	{
		/// @brief The total size of the buffer
		uint64 Size;

		ShaderBufferUniform(const string& name, ShaderStageFlags bindPoints, uint64 size);

		bool operator==(const ShaderBufferUniform& other) const;
	};
}