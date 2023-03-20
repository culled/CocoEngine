#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Matrix.h>
#include "Graphics/GraphicsPipelineState.h"
#include "Graphics/Resources/BufferTypes.h"
#include "ShaderTypes.h"

namespace Coco::Rendering
{
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
		ShaderStageType DescriptorBindingPoint;

		Subshader(
			const string& name,
			const Map<ShaderStageType, string>& stageFiles,
			const GraphicsPipelineState& pipelineState,
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors,
			const List<ShaderTextureSampler>& samplers,
			ShaderStageType bindPoint = ShaderStageType::Fragment) noexcept;

	private:
		friend class Shader;

		/// <summary>
		/// Updates this subshader's vertex attribute offsets
		/// </summary>
		void UpdateAttributeOffsets() noexcept;
	};

	/// <summary>
	/// Defines how geometry is rendered
	/// </summary>
	class COCOAPI Shader : public RenderingResource
	{
	private:
		string _name;
		List<Subshader> _subshaders;

	public:
		Shader(const string& name);
		virtual ~Shader() = default;

		/// <summary>
		/// Gets this shader's name
		/// </summary>
		/// <returns>This shader's name</returns>
		string GetName() const noexcept { return _name; }

		/// <summary>
		/// Tries to find a subshader with the given name
		/// </summary>
		/// <returns>True if a subshader was found</returns>
		bool TryGetSubshader(const string& passName, const Subshader*& subshader) const noexcept;

		/// <summary>
		/// Gets a list of all subshaders for this shader
		/// </summary>
		/// <returns>The shader's subshaders</returns>
		List<Subshader> GetSubshaders() const noexcept { return _subshaders; }

		/// <summary>
		/// Creates a subshader for this shader
		/// </summary>
		/// <param name="name">The name of the subshader</param>
		/// <param name="stageFiles">The files for each stage of the subshader</param>
		/// <param name="pipelineState">The pipeline state for the subshader</param>
		/// <param name="attributes">The attributes for the subshader</param>
		/// <param name="descriptors">The descriptors for the subshader</param>
		/// <param name="samplers">The texture samplers for the subshader</param>
		/// <param name="bindPoint">The point in this subshader when the descriptors should be bound</param>
		void CreateSubshader(
			const string& name, 
			const Map<ShaderStageType, string>& stageFiles, 
			const GraphicsPipelineState& pipelineState, 
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors,
			const List<ShaderTextureSampler>& samplers,
			ShaderStageType bindPoint = ShaderStageType::Fragment);
	};
}