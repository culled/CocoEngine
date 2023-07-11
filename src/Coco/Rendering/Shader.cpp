#include "Shader.h"

namespace Coco::Rendering
{
	Shader::Shader(ResourceID id, const string& name) : RenderingResource(id, name)
	{}

	bool Shader::TryGetSubshader(const string& name, const Subshader*& subshader) const noexcept
	{
		for (const Subshader& sub : _subshaders)
		{
			if (sub.PassName == name)
			{
				subshader = &sub;
				return true;
			}
		}

		return false;
	}

	void Shader::AddSubshader(const Subshader& subshader)
	{
		_subshaders.Add(subshader);
		_subshaders.Last().UpdateAttributeOffsets();
	}

	void Shader::CreateSubshader(
		const string& name, 
		const UnorderedMap<ShaderStageType, string>& stageFiles,
		const GraphicsPipelineState& pipelineState, 
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderDescriptor>& descriptors,
		const List<ShaderTextureSampler>& samplers,
		ShaderStageType bindPoint)
	{
		_subshaders.Construct(name, stageFiles, pipelineState, attributes, descriptors, samplers, bindPoint);
	}
}
