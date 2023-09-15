#include "Renderpch.h"
#include "RenderPassShaderTypes.h"

namespace Coco::Rendering
{
	ShaderVertexAttribute::ShaderVertexAttribute(const string& name, BufferDataType type) :
		Name(name),
		Offset(0),
		Type(type)
	{}

	bool ShaderVertexAttribute::operator==(const ShaderVertexAttribute& other) const
	{
		return Name == other.Name && Type == other.Type && Offset == other.Offset;
	}

	ShaderUniform::ShaderUniform(const string& name, UniformScope scope, ShaderStageFlags bindPoints) :
		Name(name),
		Scope(scope),
		BindingPoints(bindPoints),
		Key(ShaderUniformData::MakeKey(name.c_str()))
	{}

	bool ShaderUniform::operator==(const ShaderUniform& other) const
	{
		return Name == other.Name && Scope == other.Scope && BindingPoints == other.BindingPoints;
	}

	ShaderTextureUniform::ShaderTextureUniform(const string& name, UniformScope scope, ShaderStageFlags bindPoints, DefaultTextureType defaultTexture) :
		ShaderUniform(name, scope, bindPoints),
		DefaultTexture(defaultTexture)
	{}

	bool ShaderTextureUniform::operator==(const ShaderTextureUniform& other) const
	{
		return DefaultTexture == other.DefaultTexture && ShaderUniform::operator==(other);
	}

	ShaderDataUniform::ShaderDataUniform(const string& name, UniformScope scope, ShaderStageFlags bindPoints, BufferDataType type) :
		ShaderUniform(name, scope, bindPoints),
		Type(type)
	{}

	bool ShaderDataUniform::operator==(const ShaderDataUniform& other) const
	{
		return Type == other.Type && ShaderDataUniform::operator==(other);
	}

	ShaderStage::ShaderStage(const string& entryPointName, ShaderStageType type, const string& filePath) :
		EntryPointName(entryPointName),
		Type(type),
		FilePath(filePath)
	{}

	bool ShaderStage::operator==(const ShaderStage & other) const
	{
		return EntryPointName == other.EntryPointName && Type == other.Type && FilePath == other.FilePath;
	}
}