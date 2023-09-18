#include "Renderpch.h"
#include "RenderPassShader.h"

#include <Coco/Core/Math/Math.h>

namespace Coco::Rendering
{
	std::hash<string> _sStringHasher;

	RenderPassShader::RenderPassShader(
		const string& passName,
		const std::vector<ShaderStage>& stages,
		const GraphicsPipelineState& pipelineState,
		const std::vector<BlendState>& attachmentBlendStates,
		const std::vector<ShaderVertexAttribute>& vertexAttributes,
		const std::vector<ShaderDataUniform>& dataUniforms,
		const std::vector<ShaderTextureUniform>& textureUniforms) :
		Hash(0),
		Version(0),
		PassName(passName),
		Stages(stages),
		PipelineState(pipelineState),
		AttachmentBlendStates(attachmentBlendStates),
		VertexAttributes(vertexAttributes),
		DataUniforms(dataUniforms),
		TextureUniforms(textureUniforms)
	{
		CalculateAttributeOffsets();
		CalculateHash();
	}

	bool RenderPassShader::operator==(const RenderPassShader& other) const
	{
		return PassName == other.PassName &&
			PipelineState == other.PipelineState &&
			std::equal(Stages.cbegin(), Stages.cend(), other.Stages.cbegin()) &&
			std::equal(VertexAttributes.cbegin(), VertexAttributes.cend(), other.VertexAttributes.cbegin()) &&
			std::equal(AttachmentBlendStates.cbegin(), AttachmentBlendStates.cend(), other.AttachmentBlendStates.cbegin()) &&
			std::equal(DataUniforms.cbegin(), DataUniforms.cend(), other.DataUniforms.cbegin()) &&
			std::equal(TextureUniforms.cbegin(), TextureUniforms.cend(), other.TextureUniforms.cbegin());
	}

	std::vector<ShaderDataUniform> RenderPassShader::GetScopedDataUniforms(UniformScope scope) const
	{
		std::vector<ShaderDataUniform> scopedUniforms;

		for (const ShaderDataUniform& u : DataUniforms)
			if (u.Scope == scope)
				scopedUniforms.emplace_back(u);

		return scopedUniforms;
	}

	std::vector<ShaderTextureUniform> RenderPassShader::GetScopedTextureUniforms(UniformScope scope) const
	{
		std::vector<ShaderTextureUniform> scopedUniforms;

		for (const ShaderTextureUniform& u : TextureUniforms)
			if (u.Scope == scope)
				scopedUniforms.emplace_back(u);

		return scopedUniforms;
	}

	ShaderStageFlags RenderPassShader::GetUniformScopeBindStages(UniformScope scope) const
	{
		ShaderStageFlags flags = ShaderStageFlags::None;

		for (const ShaderDataUniform& u : DataUniforms)
			if (u.Scope == scope)
				flags |= u.BindingPoints;

		for (const ShaderTextureUniform& u : TextureUniforms)
			if (u.Scope == scope)
				flags |= u.BindingPoints;

		return flags;
	}

	bool RenderPassShader::HasScope(UniformScope scope) const
	{
		for (const auto& uniform : DataUniforms)
			if (uniform.Scope == scope)
				return true;

		for (const auto& uniform : TextureUniforms)
			if (uniform.Scope == scope)
				return true;

		return false;
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

		uint64 dataUniformHash = 0;
		for (const ShaderDataUniform& u : DataUniforms)
		{
			dataUniformHash = Math::CombineHashes(
				dataUniformHash,
				u.Key,
				static_cast<uint64>(u.Scope),
				static_cast<uint64>(u.BindingPoints),
				static_cast<uint64>(u.Type)
			);
		}

		uint64 textureUniformHash = 0;
		for (const ShaderTextureUniform& u : TextureUniforms)
		{
			textureUniformHash = Math::CombineHashes(
				textureUniformHash,
				u.Key,
				static_cast<uint64>(u.Scope),
				static_cast<uint64>(u.BindingPoints),
				static_cast<uint64>(u.DefaultTexture)
			);
		}

		Hash = Math::CombineHashes(Version, _sStringHasher(PassName), stageHash, PipelineState.GetHash(), blendHash, attrHash, dataUniformHash, textureUniformHash);
	}
}