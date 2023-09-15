#pragma once

#include "ShaderTypes.h"
#include "Graphics/GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
	/// @brief A shader for a single RenderPass
	struct RenderPassShader
	{
		/// @brief The name of the RenderPass this shader is used for
		string PassName;

		/// @brief The stages of this shader
		std::vector<ShaderStage> Stages;

		/// @brief The graphics pipeline state for this shader
		GraphicsPipelineState PipelineState;

		/// @brief The vertex attributes for this shader
		std::vector<ShaderVertexAttribute> VertexAttributes;

		/// @brief The size of each vertex's data, in bytes (auto-calculated after calling CalculateAttributeOffsets())
		uint32 VertexDataSize;

		/// @brief The data uniforms for this shader
		std::vector<ShaderDataUniform> DataUniforms;

		/// @brief The texture uniforms for this shader
		std::vector<ShaderTextureUniform> TextureUniforms;

		RenderPassShader(
			const string& passName,
			const std::vector<ShaderStage>& stages,
			const GraphicsPipelineState& pipelineState,
			const std::vector<ShaderVertexAttribute>& vertexAttributes,
			const std::vector<ShaderDataUniform>& dataUniforms,
			const std::vector<ShaderTextureUniform>& textureUniforms);

		bool operator==(const RenderPassShader& other) const;

		/// @brief Gets the data uniforms in the given scope
		/// @param scope The scope
		/// @return A list of data uniforms in the given scope
		std::vector<ShaderDataUniform> GetScopedDataUniforms(UniformScope scope) const;

		/// @brief Gets the texture uniforms in the given scope
		/// @param scope The scope
		/// @return A list of texture uniforms in the given scope
		std::vector<ShaderTextureUniform> GetScopedTextureUniforms(UniformScope scope) const;

		/// @brief Gets the binding points for a given scope
		/// @param scope The scope
		/// @return The binding points for all uniforms in the scope
		ShaderStageFlags GetUniformScopeBindStages(UniformScope scope) const;

		/// @brief Calculates the offsets for each vertex attribute and the total vertex data size
		void CalculateAttributeOffsets();
	};

	/// @brief Defines how geometry gets rendered
	class Shader
	{
	private:
		string _groupTag;
		std::vector<RenderPassShader> _passShaders;

	public:
		Shader(const string& groupTag);
		~Shader();

		/// @brief Gets this shader's group tag
		/// @return The group tag
		const string& GetGroupTag() const { return _groupTag; }

		/// @brief Adds a render pass shader to this shader
		/// @param passShader The pass shader
		void AddRenderPassShader(RenderPassShader&& passShader);

		/// @brief Gets this shader's render pass shaders
		/// @return A list of render pass shaders
		const std::vector<RenderPassShader>& GetRenderPassShaders() const { return _passShaders; }

		/// @brief Gets a render pass shader if this shader has one for the given pass
		/// @param renderPassName The name of the render pass
		/// @param outRenderPassShader Will be set to the RenderPassShader that corresponds to the given render pass name
		/// @return True if a render pass shader was found for the given render pass name
		bool TryGetRenderPassShader(const char* renderPassName, const RenderPassShader*& outRenderPassShader) const;
	};
}