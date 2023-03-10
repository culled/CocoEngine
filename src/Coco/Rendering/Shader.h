#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Resources/Resource.h>
#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Matrix.h>
#include "Graphics/GraphicsPipelineState.h"
#include "ShaderTypes.h"
#include "Graphics/BufferTypes.h"

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
		/// The binding index of the descriptor in the set
		/// </summary>
		uint BindingIndex;
		
		/// <summary>
		/// The type of descriptor
		/// </summary>
		ShaderDescriptorType Type;

		/// <summary>
		/// The point in the render pipeline when this descriptor should be bound
		/// </summary>
		ShaderStageType StageBindingPoint;

		/// <summary>
		/// The size of the descriptor container if it is a container type (e.x. struct)
		/// </summary>
		uint Size;

		ShaderDescriptor(const string& name, uint bindingIndex, ShaderDescriptorType type, uint size = 0, ShaderStageType bindingPoint = ShaderStageType::Fragment) noexcept;
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

		Subshader(
			const string& name,
			const Map<ShaderStageType, string>& stageFiles,
			const GraphicsPipelineState& pipelineState,
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors) noexcept;

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
	class COCOAPI Shader : public Resource
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
		void CreateSubshader(
			const string& name, 
			const Map<ShaderStageType, string>& stageFiles, 
			const GraphicsPipelineState& pipelineState, 
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors);
	};
}