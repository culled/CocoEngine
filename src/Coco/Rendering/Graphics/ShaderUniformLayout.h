#pragma once

#include "ShaderUniformTypes.h"
#include "ShaderUniformData.h"

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

		/// @brief The uniforms
		std::vector<ShaderUniform> Uniforms;

		ShaderUniformLayout();
		ShaderUniformLayout(const std::vector<ShaderUniform>& uniforms);

		virtual ~ShaderUniformLayout() = default;

		bool operator==(const ShaderUniformLayout& other) const;

		/// @brief Gets the default value for a uniform
		/// @param uniform The uniform
		/// @return The default value for the uniform
		static ShaderUniformUnion GetDefaultDataUniformValue(const ShaderUniform& uniform);

		/// @brief Calculates this layout's hash from its currently set values
		virtual void CalculateHash();
		
		/// @brief Gets the binding points for data or texture uniforms
		/// @param dataUniforms If true, the binding points for the data uniforms will be returned
		/// @param textureUniforms If true, the binding points for the data uniforms will be returned
		/// @return The binding points for data or texture uniforms
		ShaderStageFlags GetUniformBindStages(bool dataUniforms, bool textureUniforms) const;

		/// @brief Gets the size of the data uniforms
		/// @param device The device to calculate the size for
		/// @return The size of the uniform data
		uint64 GetUniformDataSize(const GraphicsDevice& device) const;

		/// @brief Gets buffer-friendly uniform data
		/// @param device The device to calculate the data for
		/// @param data The uniform data
		/// @param outBufferData Will be filled with the buffer-friendly data
		void GetBufferFriendlyData(const GraphicsDevice& device, const ShaderUniformData& data, std::vector<uint8>& outBufferData) const;

		/// @brief Determines if this layout has data uniforms
		/// @return True if this layout has data uniforms
		bool HasDataUniforms() const;

		/// @brief Determines if this layout has texture uniforms
		/// @return True if this layout has texture uniforms
		bool HasTextureUniforms() const;
	};

	/// @brief Represents a global layout of shader uniforms
	struct GlobalShaderUniformLayout :
		public ShaderUniformLayout
	{
		/// @brief The global buffer uniforms
		std::vector<ShaderBufferUniform> BufferUniforms;

		GlobalShaderUniformLayout();
		GlobalShaderUniformLayout(
			const std::vector<ShaderUniform>& uniforms,
			const std::vector<ShaderBufferUniform>& bufferUniforms);

		bool operator==(const GlobalShaderUniformLayout& other) const;

		void CalculateHash() final;

		/// @brief Gets the binding points for buffer uniforms
		/// @return The binding points for buffer uniforms
		ShaderStageFlags GetBufferUniformBindStages() const;
	};
}