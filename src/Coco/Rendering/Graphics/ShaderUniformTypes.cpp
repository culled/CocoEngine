#include "Renderpch.h"
#include "ShaderUniformTypes.h"

#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	BufferDataType GetBufferDataType(ShaderUniformType dataUniform)
	{
		switch (dataUniform)
		{
		case ShaderUniformType::Float:
			return BufferDataType::Float;
		case ShaderUniformType::Float2:
			return BufferDataType::Float2;
		case ShaderUniformType::Float3:
			return BufferDataType::Float3;
		case ShaderUniformType::Float4:
		case ShaderUniformType::Color:
			return BufferDataType::Float4;
		case ShaderUniformType::Mat4x4:
			return BufferDataType::Mat4x4;
		case ShaderUniformType::Int:
			return BufferDataType::Int;
		case ShaderUniformType::Int2:
			return BufferDataType::Int2;
		case ShaderUniformType::Int3:
			return BufferDataType::Int3;
		case ShaderUniformType::Int4:
			return BufferDataType::Int4;
		case ShaderUniformType::Bool:
			return BufferDataType::Bool;
		default:
			break;
		}

		// Uniform was not a data uniform
		Assert(false)
		throw std::exception("Type was not a data uniform");
	}

	bool IsDataShaderUniformType(ShaderUniformType type)
	{
		return type != ShaderUniformType::Texture;
	}

	const char* GetShaderUniformTypeString(ShaderUniformType type)
	{
		switch (type)
		{
		case ShaderUniformType::Float:
			return "Float";
		case ShaderUniformType::Float2:
			return "Float 2";
		case ShaderUniformType::Float3:
			return "Float 3";
		case ShaderUniformType::Float4:
			return "Float 4";
		case ShaderUniformType::Color:
			return "Color";
		case ShaderUniformType::Mat4x4:
			return "Matrix 4x4";
		case ShaderUniformType::Int:
			return "Int";
		case ShaderUniformType::Int2:
			return "Int 2";
		case ShaderUniformType::Int3:
			return "Int 3";
		case ShaderUniformType::Int4:
			return "Int 4";
		case ShaderUniformType::Bool:
			return "Boolean";
		case ShaderUniformType::Texture:
			return "Texture";
		default:
			return "Unknown";
		}
	}

	ShaderUniform::ShaderUniform(const string& name, ShaderUniformType type, ShaderStageFlags bindPoints, std::any defaultValue) :
		Name(name),
		Type(type),
		BindingPoints(bindPoints),
		DefaultValue(defaultValue),
		Key(ShaderUniformData::MakeKey(name.c_str()))
	{}

	bool ShaderUniform::operator==(const ShaderUniform& other) const
	{
		return Key == other.Key && Type == other.Type && BindingPoints == other.BindingPoints;
	}

	ShaderBufferUniform::ShaderBufferUniform(const string& name, ShaderStageFlags bindPoints, uint64 size) :
		Name(name),
		BindingPoints(bindPoints),
		Size(size),
		Key(ShaderUniformData::MakeKey(name.c_str()))
	{}

	bool ShaderBufferUniform::operator==(const ShaderBufferUniform& other) const
	{
		return Key == other.Key && Size == other.Size && BindingPoints == other.BindingPoints;
	}
}