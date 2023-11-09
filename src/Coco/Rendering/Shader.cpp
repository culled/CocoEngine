#include "Renderpch.h"
#include "Shader.h"

#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	std::hash<string> _sStringHasher;

	Shader::Shader(const ResourceID& id, const string& name) :
		Shader(id, name, {}, GraphicsPipelineState(), {}, VertexDataFormat(), GlobalShaderUniformLayout(), ShaderUniformLayout(), ShaderUniformLayout())
	{}

	Shader::Shader(const ResourceID& id, const string& name, const SharedRef<Shader>& shader) :
		Shader(
			id, 
			name, 
			shader->_stages, 
			shader->_pipelineState, 
			shader->_attachmentBlendStates, 
			shader->_vertexFormat, 
			shader->_globalUniforms, 
			shader->_instanceUniforms, 
			shader->_drawUniforms)
	{}

	Shader::Shader(
		const ResourceID& id,
		const string& name,
		const std::vector<ShaderStage>& stages,
		const GraphicsPipelineState& pipelineState,
		const std::vector<BlendState>& attachmentBlendStates,
		const VertexDataFormat& vertexFormat,
		const GlobalShaderUniformLayout& globalUniforms,
		const ShaderUniformLayout& instanceUniforms,
		const ShaderUniformLayout& drawUniforms) :
		RendererResource(id, name),
		_stages(stages),
		_pipelineState(pipelineState),
		_attachmentBlendStates(attachmentBlendStates),
		_vertexFormat(vertexFormat),
		_globalUniforms(globalUniforms),
		_instanceUniforms(instanceUniforms),
		_drawUniforms(drawUniforms)
	{}

	void Shader::SetStages(std::span<const ShaderStage> stages)
	{
		_stages = std::vector<ShaderStage>(stages.begin(), stages.end());
	}

	void Shader::SetPipelineState(const GraphicsPipelineState& pipelineState)
	{
		_pipelineState = pipelineState;

		IncrementVersion();
	}

	void Shader::SetAttachmentBlendStates(std::span<const BlendState> blendStates)
	{
		_attachmentBlendStates = std::vector<BlendState>(blendStates.begin(), blendStates.end());

		IncrementVersion();
	}

	void Shader::SetVertexDataFormat(const VertexDataFormat& format)
	{
		_vertexFormat = format;

		IncrementVersion();
	}

	void Shader::SetGlobalUniformLayout(const GlobalShaderUniformLayout& layout)
	{
		_globalUniforms = layout;

		IncrementVersion();
	}

	void Shader::SetInstanceUniformLayout(const ShaderUniformLayout& layout)
	{
		_instanceUniforms = layout;

		IncrementVersion();
	}

	void Shader::SetDrawUniformLayout(const ShaderUniformLayout& layout)
	{
		_drawUniforms = layout;

		IncrementVersion();
	}

	void Shader::IncrementVersion()
	{
		SetVersion(GetVersion() + 1);
	}
}