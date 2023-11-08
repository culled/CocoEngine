#pragma once

#include "Renderpch.h"
#include "RendererResource.h"
#include "Graphics/GraphicsPipelineTypes.h"
#include "Graphics/ShaderTypes.h"
#include "Graphics/ShaderUniformLayout.h"
#include "MeshTypes.h"

namespace Coco::Rendering
{
	/// @brief A shader
	class Shader :
		public RendererResource
	{
	private:
		std::vector<ShaderStage> _stages;
		GraphicsPipelineState _pipelineState;
		std::vector<BlendState> _attachmentBlendStates;
		VertexDataFormat _vertexFormat;
		GlobalShaderUniformLayout _globalUniforms;
		ShaderUniformLayout _instanceUniforms;
		ShaderUniformLayout _drawUniforms;

	public:
		Shader(const ResourceID& id, const string& name);
		Shader(const ResourceID& id, const string& name, const SharedRef<Shader>& shader);
		Shader(
			const ResourceID& id,
			const string& name,
			const std::vector<ShaderStage>& stages,
			const GraphicsPipelineState& pipelineState,
			const std::vector<BlendState>& attachmentBlendStates,
			const VertexDataFormat& vertexFormat,
			const GlobalShaderUniformLayout& globalUniforms,
			const ShaderUniformLayout& instanceUniforms,
			const ShaderUniformLayout& drawUniforms);

		std::type_index GetType() const final { return typeid(Shader); }

		/// @brief Gets the stages of this shader
		/// @return This shader's stages
		std::span<const ShaderStage> GetStages() const { return _stages; }

		/// @brief Sets the pipeline state of this shader
		/// @param pipelineState The pipeline state
		void SetPipelineState(const GraphicsPipelineState& pipelineState);

		/// @brief Gets the pipeline state of this shader
		/// @return The pipeline state
		const GraphicsPipelineState& GetPipelineState() const { return _pipelineState; }

		/// @brief Sets the attachment blend states of this shader
		/// @param blendStates The attachment blend states
		void SetAttachmentBlendStates(std::span<const BlendState> blendStates);

		/// @brief Gets the attachment blend states of this shader
		/// @return The attachment blend states
		std::span<const BlendState> GetAttachmentBlendStates() const { return _attachmentBlendStates; }

		/// @brief Sets the vertex data format of this shader
		/// @param format The vertex data format
		void SetVertexDataFormat(const VertexDataFormat& format);

		/// @brief Gets the vertex data format of this shader
		/// @return The vertex data format
		const VertexDataFormat& GetVertexDataFormat() const { return _vertexFormat; }

		/// @brief Sets the global uniform layout of this shader
		/// @param layout The global uniform layout
		void SetGlobalUniformLayout(const GlobalShaderUniformLayout& layout);

		/// @brief Gets the global uniform layout of this shader
		/// @return The global uniform layout
		const GlobalShaderUniformLayout& GetGlobalUniformLayout() const { return _globalUniforms; }

		/// @brief Sets the per-instance uniform layout of this shader
		/// @param layout The per-instance uniform layout
		void SetInstanceUniformLayout(const ShaderUniformLayout& layout);

		/// @brief Gets the per-instance uniform layout of this shader
		/// @return The per-instance uniform layout
		const ShaderUniformLayout& GetInstanceUniformLayout() const { return _instanceUniforms; }

		/// @brief Sets the per-draw uniform layout of this shader
		/// @param layout The per-draw uniform layout
		void SetDrawUniformLayout(const ShaderUniformLayout& layout);

		/// @brief Gets the per-draw uniform layout of this shader
		/// @return The per-draw uniform layout
		const ShaderUniformLayout& GetDrawUniformLayout() const { return _drawUniforms; }

	private:
		/// @brief Increments the shader version
		void IncrementVersion();
	};
}