#pragma once

#include "../Renderpch.h"
#include <Coco/Core/Defines.h>
#include "../ShaderTypes.h"
#include "ShaderUniformTypes.h"

namespace Coco::Rendering
{
	/// @brief Represents a layout of shader uniforms
	class ShaderUniformLayout
	{
	public:
		/// @brief An empty layout
		static const ShaderUniformLayout Empty;

	public:
		ShaderUniformLayout();
		ShaderUniformLayout(const std::initializer_list<ShaderUniform>& uniforms);
		ShaderUniformLayout(std::span<const ShaderUniform> uniforms);

		bool operator==(const ShaderUniformLayout& other) const { return _hash == other._hash; }

		uint64 GetHash() const { return _hash; }

		/// @brief Gets the binding points for data and/or texture uniforms
		/// @param dataUniforms If true, the binding points for the data uniforms will be included
		/// @param textureUniforms If true, the binding points for the texture uniforms will be included
		/// @return The binding points for the uniforms
		ShaderStageFlags GetUniformBindStages(bool dataUniforms, bool textureUniforms) const;

		bool NeedsDataCalculation() const;

		void CalculateDataUniforms(const GraphicsDevice& device);
		uint64 GetTotalDataSize() const { return _totalSize; }
		
		bool HasUniformsOfType(bool dataUniforms, bool textureUniforms) const;

		std::vector<const ShaderUniform*> GetUniforms(bool includeDataUniforms, bool includeTextureUniforms) const;
		std::vector<ShaderUniformValue> GetDefaultValues() const;

		std::vector<uint8> GetBufferData(std::span<const ShaderUniformValue> uniformValues) const;
		std::unordered_map<string, SharedRef<Texture>> GetTextures(std::span<const ShaderUniformValue> uniformValues) const;

	private:
		uint64 _hash;
		std::unordered_map<string, ShaderUniform> _uniforms;
		uint64 _totalSize;

	private:
		std::vector<ShaderUniform*> GetDataUniforms();
	};
}