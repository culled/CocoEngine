#pragma once

#include "ShaderUniformTypes.h"

namespace Coco::Rendering
{
	class GraphicsDevice;
	struct ShaderUniformData;

	/// @brief Represents a layout of shader uniforms
	struct ShaderUniformLayout
	{
		/// @brief A hash for an empty layout
		static const uint64 EmptyHash;

		/// @brief The hash of this layout's content (auto-calculated after calling CalculateHash())
		uint64 Hash;

		/// @brief The data uniforms
		std::vector<ShaderDataUniform> DataUniforms;

		/// @brief The texture uniforms
		std::vector<ShaderTextureUniform> TextureUniforms;

		ShaderUniformLayout();
		ShaderUniformLayout(
			const std::vector<ShaderDataUniform>& dataUniforms, 
			const std::vector<ShaderTextureUniform>& textureUniforms);

		virtual ~ShaderUniformLayout() = default;

		bool operator==(const ShaderUniformLayout& other) const;

		/// @brief Calculates this layout's hash from its currently set values
		virtual void CalculateHash();
		
		/// @brief Gets the binding points for all uniforms
		/// @return The binding points for all uniforms
		virtual ShaderStageFlags GetAllBindStages() const;

		/// @brief Gets the binding points for data uniforms
		/// @return The binding points for data uniforms
		ShaderStageFlags GetDataUniformBindStages() const;

		/// @brief Gets the binding points for texture uniforms
		/// @return The binding points for texture uniforms
		ShaderStageFlags GetTextureUniformBindStages() const;

		/// @brief Gets the size of the data uniforms
		/// @param device The device to calculate the size for
		/// @return The size of the uniform data
		uint64 GetUniformDataSize(const GraphicsDevice& device) const;

		/// @brief Gets buffer-friendly uniform data
		/// @param device The device to calculate the data for
		/// @param data The uniform data
		/// @return The buffer-friendly data
		std::vector<uint8> GetBufferFriendlyData(const GraphicsDevice& device, const ShaderUniformData& data) const;
	};

	/// @brief Represents a global layout of shader uniforms
	struct GlobalShaderUniformLayout :
		public ShaderUniformLayout
	{
		/// @brief The global buffer uniforms
		std::vector<ShaderBufferUniform> BufferUniforms;

		GlobalShaderUniformLayout();
		GlobalShaderUniformLayout(
			const std::vector<ShaderDataUniform>& dataUniforms,
			const std::vector<ShaderTextureUniform>& textureUniforms,
			const std::vector<ShaderBufferUniform>& bufferUniforms);

		bool operator==(const GlobalShaderUniformLayout& other) const;

		void CalculateHash() final;
		ShaderStageFlags GetAllBindStages() const final;

		/// @brief Gets the binding points for buffer uniforms
		/// @return The binding points for buffer uniforms
		ShaderStageFlags GetBufferUniformBindStages() const;
	};
}