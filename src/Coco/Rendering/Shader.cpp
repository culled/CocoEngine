#include "Renderpch.h"
#include "Shader.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	Shader::Shader(const ResourceID& id, const string& name) :
		Shader(id, name, {}, GraphicsPipelineState(), {}, VertexDataFormat(), ShaderUniformLayout::Empty, ShaderUniformLayout::Empty, ShaderUniformLayout::Empty)
	{}

	Shader::Shader(
		const ResourceID& id,
		const string& name,
		const std::vector<ShaderStage>& stages,
		const GraphicsPipelineState& pipelineState,
		const std::vector<AttachmentBlendState>& attachmentBlendStates,
		const VertexDataFormat& vertexFormat,
		const ShaderUniformLayout& globalUniforms,
		const ShaderUniformLayout& instanceUniforms,
		const ShaderUniformLayout& drawUniforms) :
		Resource(id),
		_name(name),
		_stages(stages),
		_pipelineState(pipelineState),
		_attachmentBlendStates(attachmentBlendStates),
		_vertexFormat(vertexFormat),
		_globalUniforms(globalUniforms),
		_instanceUniforms(instanceUniforms),
		_drawUniforms(drawUniforms)
	{
		EnsureLayoutDataCalculated();
	}

	void Shader::EnsureLayoutDataCalculated()
	{
		RenderService* rendering = RenderService::Get();
		CocoAssert(rendering, "RenderService singleton was null")

		GraphicsDevice& device = rendering->GetDevice();

		if (_globalUniforms.NeedsDataCalculation())
			_globalUniforms.CalculateDataUniforms(device);

		if (_instanceUniforms.NeedsDataCalculation())
			_instanceUniforms.CalculateDataUniforms(device);

		if (_drawUniforms.NeedsDataCalculation())
			_drawUniforms.CalculateDataUniforms(device);
	}
}