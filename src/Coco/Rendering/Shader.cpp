#include "Shader.h"

namespace Coco::Rendering
{
	ShaderVertexAttribute::ShaderVertexAttribute(BufferDataFormat dataFormat) :
		DataFormat(dataFormat), DataOffset(0)
	{}

	ShaderDescriptor::ShaderDescriptor(const string & name, uint bindingIndex, ShaderDescriptorType type, uint size, ShaderStageType bindingPoint) :
		Name(name), BindingIndex(bindingIndex), Type(type), Size(size), StageBindingPoint(bindingPoint)
	{}

	Subshader::Subshader(
		const string& name,
		const Map<ShaderStageType, string>& stageFiles,
		const GraphicsPipelineState& pipelineState,
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderDescriptor>& descriptors) :
		PassName(name), StageFiles(stageFiles), PipelineState(pipelineState), Attributes(attributes), Descriptors(descriptors)
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
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderDescriptor>& descriptors)
	{
		_subshaders.Add(Subshader(name, stageFiles, pipelineState, attributes, descriptors));
	}
}
