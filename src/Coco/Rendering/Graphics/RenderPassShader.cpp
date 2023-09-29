#include "Renderpch.h"
#include "RenderPassShader.h"

#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	std::hash<string> _sStringHasher;

	RenderPassShader::RenderPassShader(
		uint64 baseID,
		const string& passName,
		const std::vector<ShaderStage>& stages,
		const GraphicsPipelineState& pipelineState,
		const std::vector<BlendState>& attachmentBlendStates,
		const std::vector<ShaderVertexAttribute>& vertexAttributes,
		const GlobalShaderUniformLayout& globalUniforms,
		const ShaderUniformLayout& instanceUniforms,
		const ShaderUniformLayout& drawUniforms) :
		ID(Math::CombineHashes(baseID, _sStringHasher(passName))),
		Hash(0),
		PassName(passName),
		Stages(stages),
		PipelineState(pipelineState),
		AttachmentBlendStates(attachmentBlendStates),
		VertexAttributes(vertexAttributes),
		GlobalUniforms(globalUniforms),
		InstanceUniforms(instanceUniforms),
		DrawUniforms(drawUniforms)
	{
		CalculateAttributeOffsets();
		CalculateHash();
	}

	bool RenderPassShader::operator==(const RenderPassShader& other) const
	{
		return Hash == other.Hash;
	}

	void RenderPassShader::CalculateAttributeOffsets()
	{
		uint32 offset = 0;
		for (ShaderVertexAttribute& attr : VertexAttributes)
		{
			attr.Offset = offset;
			offset += GetDataTypeSize(attr.Type);
		}

		VertexDataSize = offset;
	}

	void RenderPassShader::CalculateHash()
	{
		uint64 stageHash = 0;
		for (const ShaderStage& stage : Stages)
		{
			stageHash = Math::CombineHashes(
				stageHash,
				_sStringHasher(stage.EntryPointName),
				static_cast<uint64>(stage.Type),
				_sStringHasher(stage.FilePath)
			);
		}

		uint64 attrHash = 0;
		for (const ShaderVertexAttribute& attr : VertexAttributes)
		{
			attrHash = Math::CombineHashes(
				attrHash,
				_sStringHasher(attr.Name),
				attr.Offset,
				static_cast<uint64>(attr.Type)
			);
		}

		uint64 blendHash = 0;
		for (const BlendState& state : AttachmentBlendStates)
		{
			blendHash = Math::CombineHashes(blendHash, state.GetHash());
		}

		Hash = Math::CombineHashes(
			_sStringHasher(PassName), 
			stageHash, 
			PipelineState.GetHash(), 
			blendHash, 
			attrHash, 
			GlobalUniforms.Hash, 
			InstanceUniforms.Hash,
			DrawUniforms.Hash);
	}
}