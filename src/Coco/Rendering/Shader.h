#pragma once

#include "RenderingResource.h"

#include <Coco/Core/Types/Map.h>
#include <Coco/Core/Types/List.h>
#include <Coco/Core/Types/Matrix.h>
#include "ShaderTypes.h"

namespace Coco::Rendering
{
	/// @brief Defines how geometry is rendered
	class COCOAPI Shader : public RenderingResource
	{
	private:
		List<Subshader> _subshaders;
		string _groupTag;

	public:
		Shader(const ResourceID& id, const string& name);

		DefineResourceType(Shader)

		/// @brief Tries to find a subshader with the given name
		/// @param name The name of the subshader
		/// @param subshader Will be filled with the subshader if it was found
		/// @return True if a subshader was found
		bool TryGetSubshader(const string& name, const Subshader*& subshader) const noexcept;

		/// @brief Gets all subshaders for this shader
		/// @return This shader's subshaders
		const List<Subshader>& GetSubshaders() const noexcept { return _subshaders; }

		/// @brief Adds a subshader to this shader
		/// @param subshader The subshader to add
		void AddSubshader(const Subshader& subshader);

		/// @brief Creates a subshader for this shader
		/// @param name The name of the subshader
		/// @param stages The stages for the subshader
		/// @param pipelineState The pipeline state for the subshader
		/// @param attributes The attributes for the subshader
		/// @param uniforms The uniforms for the subshader
		/// @param samplers The texture samplers for the subshader
		void CreateSubshader(
			const string& name, 
			const List<ShaderStage>& stages,
			const GraphicsPipelineState& pipelineState, 
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderUniformDescriptor>& uniforms,
			const List<ShaderTextureSampler>& samplers);

		/// @brief Sets the tag for this shader
		/// @param groupTag The tag
		void SetGroupTag(const string& groupTag) { _groupTag = groupTag; }

		/// @brief Gets the tag for this shader
		/// @return This shader's tag
		const string& GetGroupTag() const { return _groupTag; }

		/// @brief Gets a shader uniform data object with empty uniforms for this shader
		/// @return An empty shader uniform object
		ShaderUniformData GetUniformPropertyMap() const;
	};
}