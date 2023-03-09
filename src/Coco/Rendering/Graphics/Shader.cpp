#include "Shader.h"

namespace Coco::Rendering
{
	ShaderVertexAttribute::ShaderVertexAttribute(BufferDataFormat dataFormat) :
		DataFormat(dataFormat), DataOffset(0)
	{}

	Subshader::Subshader(
		const string& name,
		const Map<ShaderStageType, string>& stageFiles,
		const GraphicsPipelineState& pipelineState,
		const List<ShaderVertexAttribute>& attributes) :
		PassName(name), StageFiles(stageFiles), PipelineState(pipelineState), Attributes(attributes)
	{
		UpdateAttributeOffsets();
	}

	void Subshader::UpdateAttributeOffsets()
	{
		uint32_t offset = 0;

		for (ShaderVertexAttribute& attr : Attributes)
		{
			attr.DataOffset = offset;
			offset += GetBufferDataFormatSize(attr.DataFormat);
		}
	}

	Shader::Shader(const string& name) :
		_name(name)
	{}

	bool Shader::TryGetSubshader(const string & passName, const Subshader** subshader) const
	{
		for (const Subshader& sub : _subshaders)
		{
			if (sub.PassName == passName)
			{
				*subshader = &sub;
				return true;
			}
		}

		subshader = nullptr;
		return false;
	}

	void Shader::CreateSubshader(
		const string& name, 
		const Map<ShaderStageType, string>& stageFiles, 
		const GraphicsPipelineState& pipelineState, 
		const List<ShaderVertexAttribute>& attributes)
	{
		_subshaders.Add(Subshader(name, stageFiles, pipelineState, attributes));
	}
}
