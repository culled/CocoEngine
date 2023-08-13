#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Types/String.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "Graphics/Resources/BufferTypes.h"
#include "Graphics/GraphicsPipelineState.h"

namespace Coco::Rendering
{
	/// @brief Types of shader stages
	enum class ShaderStageType
	{
		Vertex,
		Tesselation,
		Geometry,
		Fragment,
		Compute
	};

	/// @brief Types of shader descriptors
	enum class ShaderDescriptorType
	{
		UniformColor,
		UniformVector4
	};

	/// @brief An attribute that represents a kind of data within a contiguous vertex buffer
	struct COCOAPI ShaderVertexAttribute
	{
		friend struct Subshader;

	private:
		/// @brief The auto-calculated offset from the start of the vertex data structure of this attribute
		uint32_t _dataOffset;

	public:
		/// @brief The format of the data for this attribute
		BufferDataFormat DataFormat;

		ShaderVertexAttribute(BufferDataFormat dataFormat) noexcept;

		/// @brief Gets the auto-calculated offset from the start of the vertex data structure of this attribute
		/// @return The offset from the start of the vertex data structure of this attribute
		constexpr uint32_t GetDataOffset() const noexcept { return _dataOffset; }
	};

	/// @brief A descriptor for a shader
	struct COCOAPI ShaderDescriptor
	{
		/// @brief The descriptor name (used for referencing from materials)
		string Name;

		/// @brief The type of descriptor
		BufferDataFormat Type;

		ShaderDescriptor(const string& name, BufferDataFormat type) noexcept;
	};

	/// @brief A texture sampler for a shader
	struct COCOAPI ShaderTextureSampler
	{
		/// @brief The descriptor name (used for referencing from materials)
		string Name;

		ShaderTextureSampler(const string& name) noexcept;
	};

	/// @brief A render-pass specific shader
	struct COCOAPI Subshader
	{
		friend class Shader;

		/// @brief The name of the render pass that uses this subshader
		string PassName;

		/// @brief Files for each stage of this subshader
		UnorderedMap<ShaderStageType, string> StageFiles;

		/// @brief The graphics pipeline state that this subshader requires
		GraphicsPipelineState PipelineState;

		/// @brief Vertex shader attributes for this subshader
		List<ShaderVertexAttribute> Attributes;

		/// @brief Descriptors for this subshader
		List<ShaderDescriptor> Descriptors;

		/// @brief Texture samplers for this subshader
		List<ShaderTextureSampler> Samplers;

		/// @brief The point in the render pipeline when the descriptors should be bound
		ShaderStageType DescriptorBindingPoint = ShaderStageType::Fragment;

		Subshader() = default;

		Subshader(
			const string& name,
			const UnorderedMap<ShaderStageType, string>& stageFiles,
			const GraphicsPipelineState& pipelineState,
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors,
			const List<ShaderTextureSampler>& samplers,
			ShaderStageType bindPoint = ShaderStageType::Fragment) noexcept;

	private:
		/// @brief Updates this subshader's vertex attribute offsets
		void UpdateAttributeOffsets() noexcept;
	};
}