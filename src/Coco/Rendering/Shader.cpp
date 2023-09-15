#include "Renderpch.h"
#include "Shader.h"

namespace Coco::Rendering
{
	RenderPassShader::RenderPassShader(
		const string& passName, 
		const std::vector<ShaderStage>& stages, 
		const GraphicsPipelineState& pipelineState, 
		const std::vector<ShaderVertexAttribute>& vertexAttributes, 
		const std::vector<ShaderDataUniform>& dataUniforms, 
		const std::vector<ShaderTextureUniform>& textureUniforms) :
		PassName(passName),
		Stages(stages),
		PipelineState(pipelineState),
		VertexAttributes(vertexAttributes),
		DataUniforms(dataUniforms),
		TextureUniforms(textureUniforms)
	{
		CalculateAttributeOffsets();
	}

	bool RenderPassShader::operator==(const RenderPassShader& other) const
	{
		return PassName == other.PassName &&
			PipelineState == other.PipelineState &&
			std::equal(Stages.cbegin(), Stages.cend(), other.Stages.cbegin()) &&
			std::equal(VertexAttributes.cbegin(), VertexAttributes.cend(), other.VertexAttributes.cbegin()) &&
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

	Shader::Shader(const string& groupTag) :
		_groupTag(groupTag),
		_passShaders{}
	{}

	Shader::~Shader()
	{
		_passShaders.clear();
	}

	void Shader::AddRenderPassShader(RenderPassShader&& passShader)
	{
		_passShaders.emplace_back(std::forward<RenderPassShader>(passShader));
	}

	bool Shader::TryGetRenderPassShader(const char* renderPassName, const RenderPassShader*& outRenderPassShader) const
	{
		for (const RenderPassShader& passShader : _passShaders)
		{
			if (passShader.PassName == renderPassName)
			{
				outRenderPassShader = &passShader;
				return true;
			}
		}

		return false;
	}
}