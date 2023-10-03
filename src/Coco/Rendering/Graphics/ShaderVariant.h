#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Types/String.h>
#include "GraphicsPipelineTypes.h"
#include "ShaderTypes.h"
#include "ShaderUniformLayout.h"

namespace Coco::Rendering
{
	/// @brief A variant of a shader
	struct ShaderVariant
	{
		/// @brief The hash of this shader's values (auto-calculated after calling CalculateHash())
		uint64 Hash;

		/// @brief The name of this shader variant
		string Name;

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

		/// @brief The layout for the shader global uniforms
		GlobalShaderUniformLayout GlobalUniforms;

		/// @brief The layout for instance uniforms
		ShaderUniformLayout InstanceUniforms;

		/// @brief The layout for draw uniforms
		ShaderUniformLayout DrawUniforms;

		ShaderVariant(
			const string& name,
			const std::vector<ShaderStage>& stages,
			const GraphicsPipelineState& pipelineState,
			const std::vector<BlendState>& attachmentBlendStates,
			const std::vector<ShaderVertexAttribute>& vertexAttributes,
			const GlobalShaderUniformLayout& globalUniforms,
			const ShaderUniformLayout& instanceUniforms,
			const ShaderUniformLayout& drawUniforms);

		bool operator==(const ShaderVariant& other) const;

		/// @brief Calculates the offsets for each vertex attribute and the total vertex data size
		void CalculateAttributeOffsets();

		/// @brief Calculates this shader's hash from its currently set values
		void CalculateHash();
	};
}