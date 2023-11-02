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
		const VertexDataFormat& vertexFormat,
		const GlobalShaderUniformLayout& globalUniforms,
		const ShaderUniformLayout& instanceUniforms,
		const ShaderUniformLayout& drawUniforms) :
		Hash(0),
		Name(name),
		Stages(stages),
		PipelineState(pipelineState),
		AttachmentBlendStates(attachmentBlendStates),
		VertexFormat(vertexFormat),
		GlobalUniforms(globalUniforms),
		InstanceUniforms(instanceUniforms),
		DrawUniforms(drawUniforms)
	{
		CalculateHash();
	}

	bool ShaderVariant::operator==(const ShaderVariant& other) const
	{
		return Hash == other.Hash;
	}

	void ShaderVariant::CalculateHash()
	{
		uint64 stageHash = std::accumulate(Stages.begin(), Stages.end(), static_cast<uint64>(0), 
			[](uint64 hash, const ShaderStage& stage) 
			{
				return Math::CombineHashes(
					hash,
					_sStringHasher(stage.EntryPointName),
					static_cast<uint64>(stage.Type),
					_sStringHasher(stage.SourceFilePath)
				);
			});

		uint64 attrHash = 0;
		VertexFormat.ForEachAdditionalAttr([&attrHash](VertexAttrFlags flag, BufferDataType type)
			{
				attrHash |= static_cast<uint64>(flag);
			});

		uint64 blendHash = std::accumulate(AttachmentBlendStates.begin(), AttachmentBlendStates.end(), static_cast<uint64>(0),
			[](uint64 hash, const BlendState& state)
			{
				return Math::CombineHashes(hash, state.GetHash());
			}
		);

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