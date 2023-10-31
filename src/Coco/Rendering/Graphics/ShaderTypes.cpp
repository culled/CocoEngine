#include "Renderpch.h"
#include "ShaderTypes.h"

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

	ShaderStage::ShaderStage(const string& entryPointName, ShaderStageType type, const string& filePath) :
		ShaderStage(entryPointName, type, filePath, string())
	{}

	ShaderStage::ShaderStage(const string& entryPointName, ShaderStageType type, const string& filePath, const string& compiledFilePath) :
		EntryPointName(entryPointName),
		Type(type),
		SourceFilePath(filePath),
		CompiledFilePath(compiledFilePath)
	{}

	bool ShaderStage::operator==(const ShaderStage & other) const
	{
		return EntryPointName == other.EntryPointName && Type == other.Type && SourceFilePath == other.SourceFilePath;
	}
}