#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Matrix.h>
#include "ShaderTypes.h"

namespace Coco::Rendering
{
	/// <summary>
	/// Defines how geometry is rendered
	/// </summary>
	class COCOAPI Shader : public RenderingResource
	{
	private:
		List<Subshader> _subshaders;

	public:
		Shader(const string& name = "");
		virtual ~Shader() = default;

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
		/// Adds a subshader to this shader
		/// </summary>
		/// <param name="subshader">The subshader to add</param>
		void AddSubshader(const Subshader& subshader) { _subshaders.Add(subshader); }

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