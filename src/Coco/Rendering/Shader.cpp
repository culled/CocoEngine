#include "Shader.h"

namespace Coco::Rendering
{
	Shader::Shader(const string& name) : RenderingResource(name, ResourceType::Shader)
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
