#include "Renderpch.h"
#include "ShaderUniformTypes.h"

#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	ShaderUniform::ShaderUniform(const string& name, ShaderStageFlags bindPoints) :
		Name(name),
		BindingPoints(bindPoints),
		Key(ShaderUniformData::MakeKey(name.c_str()))
	{}

	bool ShaderUniform::operator==(const ShaderUniform& other) const
	{
		return Key == other.Key && BindingPoints == other.BindingPoints;
	}

	ShaderTextureUniform::ShaderTextureUniform(const string& name, ShaderStageFlags bindPoints, DefaultTextureType defaultTexture) :
		ShaderUniform(name, bindPoints),
		DefaultTexture(defaultTexture)
	{}

	bool ShaderTextureUniform::operator==(const ShaderTextureUniform& other) const
	{
		return DefaultTexture == other.DefaultTexture && ShaderUniform::operator==(other);
	}

	ShaderDataUniform::ShaderDataUniform(const string& name, ShaderStageFlags bindPoints, BufferDataType type) :
		ShaderUniform(name, bindPoints),
		Type(type)
	{}

	bool ShaderDataUniform::operator==(const ShaderDataUniform& other) const
	{
		return Type == other.Type && ShaderDataUniform::operator==(other);
	}

	ShaderBufferUniform::ShaderBufferUniform(const string& name, ShaderStageFlags bindPoints, uint64 size) :
		ShaderUniform(name, bindPoints),
		Size(size)
	{}

	bool ShaderBufferUniform::operator==(const ShaderBufferUniform& other) const
	{
		return Size == other.Size && ShaderUniform::operator==(other);
	}

}