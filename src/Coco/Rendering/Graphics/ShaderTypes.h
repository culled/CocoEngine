#pragma once

#include <Coco/Core/Types/String.h>
#include "BufferTypes.h"
#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	/// @brief Shader stage flags
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

	ShaderStageFlags ToShaderStageFlags(ShaderStageType stage);

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

	/// @brief A stage within a shader
	struct ShaderStage
	{
		/// @brief The name of the stage's entry point
		string EntryPointName;

		/// @brief The type of this stage
		ShaderStageType Type;

		/// @brief The source file for the stage
		string SourceFilePath;

		/// @brief The compiled shader file path
		string CompiledFilePath;

		ShaderStage(const string& entryPointName, ShaderStageType type, const string& filePath);
		ShaderStage(const string& entryPointName, ShaderStageType type, const string& filePath, const string& compiledFilePath);

		bool operator==(const ShaderStage& other) const;
	};
}