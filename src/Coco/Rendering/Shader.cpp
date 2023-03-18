#include "Shader.h"

namespace Coco::Rendering
{
	ShaderVertexAttribute::ShaderVertexAttribute(BufferDataFormat dataFormat) noexcept :
		DataFormat(dataFormat), DataOffset(0)
	{}

	ShaderDescriptor::ShaderDescriptor(const string & name, BufferDataFormat type) noexcept :
		Name(name), Type(type)
	{}

	ShaderTextureSampler::ShaderTextureSampler(const string & name) noexcept :
		Name(name)
	{}

	Subshader::Subshader(
		const string& name,
		const Map<ShaderStageType, string>& stageFiles,
		const GraphicsPipelineState& pipelineState,
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderDescriptor>& descriptors,
		const List<ShaderTextureSampler>& samplers,
		ShaderStageType bindPoint) noexcept :
		PassName(name), 
		StageFiles(stageFiles), 
		PipelineState(pipelineState), 
		Attributes(attributes), 
		Descriptors(descriptors), 
		Samplers(samplers), 
		DescriptorBindingPoint(bindPoint)
	{
		UpdateAttributeOffsets();
	}

	void Subshader::UpdateAttributeOffsets() noexcept
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

	bool Shader::TryGetSubshader(const string & passName, const Subshader*& subshader) const noexcept
	{
		for (const Subshader& sub : _subshaders)
		{
			if (sub.PassName == passName)
			{
				subshader = &sub;
				return true;
			}
		}

		return false;
	}

	void Shader::CreateSubshader(
		const string& name, 
		const Map<ShaderStageType, string>& stageFiles, 
		const GraphicsPipelineState& pipelineState, 
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderDescriptor>& descriptors,
		const List<ShaderTextureSampler>& samplers,
		ShaderStageType bindPoint)
	{
		_subshaders.Add(Subshader(name, stageFiles, pipelineState, attributes, descriptors, samplers, bindPoint));
	}
}
