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

	public:
		Shader(ResourceID id, const string& name);

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
		/// @param stageFiles The files for each stage of the subshader
		/// @param pipelineState The pipeline state for the subshader
		/// @param attributes The attributes for the subshader
		/// @param descriptors The descriptors for the subshader
		/// @param samplers The texture samplers for the subshader
		/// @param bindPoint The point in this subshader when the descriptors should be bound
		void CreateSubshader(
			const string& name, 
			const UnorderedMap<ShaderStageType, string>& stageFiles,
			const GraphicsPipelineState& pipelineState, 
			const List<ShaderVertexAttribute>& attributes,
			const List<ShaderDescriptor>& descriptors,
			const List<ShaderTextureSampler>& samplers,
			ShaderStageType bindPoint = ShaderStageType::Fragment);
	};
}