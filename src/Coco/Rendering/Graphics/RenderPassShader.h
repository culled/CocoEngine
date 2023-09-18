#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/String.h>
#include "GraphicsPipelineTypes.h"
#include "RenderPassShaderTypes.h"

namespace Coco::Rendering
{
	/// @brief A shader for a single RenderPass
	struct RenderPassShader
	{
		/// @brief The hash of this shader
		uint64 Hash;

		/// @brief The version of this shader
		uint64 Version;

		/// @brief The name of the RenderPass this shader is used for
		string PassName;

		/// @brief The stages of this shader
		std::vector<ShaderStage> Stages;

		/// @brief The graphics pipeline state for this shader
		GraphicsPipelineState PipelineState;

		/// @brief Blending states for this shader's attachments
		std::vector<BlendState> AttachmentBlendStates;

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
			const std::vector<BlendState>& attachmentBlendStates,
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

		/// @brief Determines if this shader has any uniforms for the given scope
		/// @return True if this shader has any uniforms for the given scope
		bool HasScope(UniformScope scope) const;

		/// @brief Calculates the offsets for each vertex attribute and the total vertex data size
		void CalculateAttributeOffsets();

	private:
		void CalculateHash();
	};
}