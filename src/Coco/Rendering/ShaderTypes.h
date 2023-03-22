#pragma once

#include <Coco/Core/Core.h>

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include "Graphics/Resources/BufferTypes.h"
#include "Graphics/GraphicsPipelineState.h"

namespace Coco::Rendering
{
	/// <summary>
	/// Types of shader stages
	/// </summary>
	enum class ShaderStageType
	{
		Vertex,
		Tesselation,
		Geometry,
		Fragment,
		Compute
	};

	/// <summary>
	/// Types of shader descriptors
	/// </summary>
	enum class ShaderDescriptorType
	{
		UniformVector4
	};

	/// <summary>
	/// An attribute that represents a kind of data within a contiguous vertex buffer
	/// </summary>
	struct COCOAPI ShaderVertexAttribute
	{
		/// <summary>
		/// The format of the data for this attribute
		/// </summary>
		BufferDataFormat DataFormat;

		/// <summary>
		/// The auto-calculated offset from the start of the vertex data structure of this attribute
		/// </summary>
		uint32_t DataOffset;

		ShaderVertexAttribute(BufferDataFormat dataFormat) noexcept;
	};

	/// <summary>
	/// A descriptor for a shader
	/// </summary>
	struct COCOAPI ShaderDescriptor
	{
		/// <summary>
		/// The descriptor name (used for referencing from materials)
		/// </summary>
		string Name;

		/// <summary>
		/// The type of descriptor
		/// </summary>
		BufferDataFormat Type;

		ShaderDescriptor(const string& name, BufferDataFormat type) noexcept;
	};

	/// <summary>
	/// A texture sampler for a shader
	/// </summary>
	struct COCOAPI ShaderTextureSampler
	{
		/// <summary>
		/// The descriptor name (used for referencing from materials)
		/// </summary>
		string Name;

		ShaderTextureSampler(const string& name) noexcept;
	};

	/// <summary>
	/// A render-pass specific shader
	/// </summary>
	struct COCOAPI Subshader
	{
		/// <summary>
		/// The name of the render pass that uses this subshader
		/// </summary>
		string PassName;

		/// <summary>
		/// Files for each stage of this subshader
		/// </summary>
		Map<ShaderStageType, string> StageFiles;

		/// <summary>
		/// The graphics pipeline state that this subshader requires
		/// </summary>
		GraphicsPipelineState PipelineState;

		/// <summary>
		/// Vertex shader attributes for this subshader
		/// </summary>
		List<ShaderVertexAttribute> Attributes;

		/// <summary>
		/// Descriptors for this subshader
		/// </summary>
		List<ShaderDescriptor> Descriptors;

		/// <summary>
		/// Texture samplers for this subshader
		/// </summary>
		List<ShaderTextureSampler> Samplers;

		/// <summary>
		/// The point in the render pipeline when the descriptors should be bound
		/// </summary>
		ShaderStageType DescriptorBindingPoint = ShaderStageType::Fragment;

		Subshader() = default;

		Subshader(
			const string& name,
			const Map<ShaderStageType, string>& stageFiles,
			const GraphicsPipelineState& pipelineState,
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors,
			const List<ShaderTextureSampler>& samplers,
			ShaderStageType bindPoint = ShaderStageType::Fragment) noexcept;

		/// <summary>
		/// Updates this subshader's vertex attribute offsets
		/// </summary>
		void UpdateAttributeOffsets() noexcept;
	};

}