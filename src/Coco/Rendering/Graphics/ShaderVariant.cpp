#include "Renderpch.h"
#include "ShaderVariant.h"

#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	std::hash<string> _sStringHasher;

	ShaderVariant::ShaderVariant(
		const string& name,
		const std::vector<ShaderStage>& stages,
		const GraphicsPipelineState& pipelineState,
		const std::vector<BlendState>& attachmentBlendStates,
		const std::vector<ShaderVertexAttribute>& vertexAttributes,
		const GlobalShaderUniformLayout& globalUniforms,
		const ShaderUniformLayout& instanceUniforms,
		const ShaderUniformLayout& drawUniforms) :
		Hash(0),
		Name(name),
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

	bool ShaderVariant::operator==(const ShaderVariant& other) const
	{
		return Hash == other.Hash;
	}

	void ShaderVariant::CalculateAttributeOffsets()
	{
		uint32 offset = 0;
		for (ShaderVertexAttribute& attr : VertexAttributes)
		{
			attr.Offset = offset;
			offset += GetDataTypeSize(attr.Type);
		}

		VertexDataSize = offset;
	}

	void ShaderVariant::CalculateHash()
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
			_sStringHasher(Name), 
			stageHash, 
			PipelineState.GetHash(), 
			blendHash, 
			attrHash, 
			GlobalUniforms.Hash, 
			InstanceUniforms.Hash,
			DrawUniforms.Hash);
	}
}