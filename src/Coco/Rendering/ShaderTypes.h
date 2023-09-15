#pragma once

#include <Coco/Core/Types/String.h>
#include "Graphics/BufferTypes.h"
#include "Graphics/ShaderUniformData.h"

namespace Coco::Rendering
{
	/// @brief Types of shader stages
	enum class ShaderStageFlags
	{
		None = 0,
		Vertex = 1 << 0,
		Tesselation = 1 << 1,
		Geometry = 1 << 2,
		Fragment = 1 << 3,
		Compute = 1 << 4,
	};

	/// @brief Types of shader stages
	enum class ShaderStageType
	{
		Vertex,
		Tesselation,
		Geometry,
		Fragment,
		Compute
	};

	constexpr ShaderStageFlags operator|(const ShaderStageFlags& a, const ShaderStageFlags& b) { return static_cast<ShaderStageFlags>(static_cast<int>(a) | static_cast<int>(b)); }
	constexpr ShaderStageFlags operator&(const ShaderStageFlags& a, const ShaderStageFlags& b) { return static_cast<ShaderStageFlags>(static_cast<int>(a) & static_cast<int>(b)); }

	constexpr void operator|=(ShaderStageFlags& a, const ShaderStageFlags& b) { a = a | b; }
	constexpr void operator&=(ShaderStageFlags& a, const ShaderStageFlags& b) { a = a & b; }

	/// @brief An attribute that represents a kind of data within a contiguous vertex buffer
	struct ShaderVertexAttribute
	{
		/// @brief The name of this attribute
		string Name;

		/// @brief The offset from the start of the vertex data structure of this attribute
		uint32 Offset;

		/// @brief The type of the data for this attribute
		BufferDataType Type;

		ShaderVertexAttribute(const string& name, BufferDataType type);

		bool operator==(const ShaderVertexAttribute& other) const;
	};

	/// @brief A uniform for a shader
	struct ShaderUniform
	{
		/// @brief The uniform name (used for referencing from materials)
		string Name;

		/// @brief The scope of this descriptor
		UniformScope Scope;

		/// @brief The points in the rendering stage when this uniform should be bound
		ShaderStageFlags BindingPoints;

		/// @brief The auto-generated key from the name
		ShaderUniformData::UniformKey Key;

		ShaderUniform(const string& name, UniformScope scope, ShaderStageFlags bindPoints);

		virtual ~ShaderUniform() = default;

		bool operator==(const ShaderUniform& other) const;
	};

	/// @brief A texture uniform for a shader
	struct ShaderTextureUniform : public ShaderUniform
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

		ShaderTextureUniform(const string& name, UniformScope scope, ShaderStageFlags bindPoints, DefaultTextureType defaultTexture = DefaultTextureType::White);

		bool operator==(const ShaderTextureUniform& other) const;
	};

	/// @brief A data uniform for a shader
	struct ShaderDataUniform : public ShaderUniform
	{
		/// @brief The type of data
		BufferDataType Type;

		ShaderDataUniform(const string& name, UniformScope scope, ShaderStageFlags bindPoints, BufferDataType type);

		bool operator==(const ShaderDataUniform& other) const;
	};

	/// @brief A stage within a shader
	struct ShaderStage
	{
		/// @brief The name of the stage's entry point
		string EntryPointName;

		/// @brief The type of this stage
		ShaderStageType Type;

		/// @brief The file for the stage
		string FilePath;

		ShaderStage(const string& entryPointName, ShaderStageType type, const string& filePath);

		bool operator==(const ShaderStage& other) const;
	};
}