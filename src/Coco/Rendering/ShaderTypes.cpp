#include "Renderpch.h"
#include "ShaderTypes.h"

namespace Coco::Rendering
{
	ShaderStageFlags ToShaderStageFlags(ShaderStageType stage)
	{
		switch (stage)
		{
		case ShaderStageType::Vertex:
			return ShaderStageFlags::Vertex;
		case ShaderStageType::Tesselation:
			return ShaderStageFlags::Tesselation;
		case ShaderStageType::Geometry:
			return ShaderStageFlags::Geometry;
		case ShaderStageType::Fragment:
			return ShaderStageFlags::Fragment;
		case ShaderStageType::Compute:
			return ShaderStageFlags::Compute;
		default:
			return ShaderStageFlags::None;
		}
	}

	ShaderVertexAttribute::ShaderVertexAttribute(const string& name, BufferDataType type) :
		Name(name),
		Offset(0),
		Type(type)
	{}

	bool ShaderVertexAttribute::operator==(const ShaderVertexAttribute & other) const
	{
		return Name == other.Name && Type == other.Type && Offset == other.Offset;
	}

	ShaderStage::ShaderStage(const string& entryPointName, ShaderStageType type, const FilePath& filePath) :
		ShaderStage(entryPointName, type, filePath, string())
	{}

	ShaderStage::ShaderStage(const string& entryPointName, ShaderStageType type, const FilePath& filePath, const FilePath& compiledFilePath) :
		EntryPointName(entryPointName),
		Type(type),
		SourceFilePath(filePath),
		CompiledFilePath(compiledFilePath)
	{}

	bool ShaderStage::operator==(const ShaderStage& other) const
	{
		return EntryPointName == other.EntryPointName && Type == other.Type && SourceFilePath == other.SourceFilePath;
	}
}