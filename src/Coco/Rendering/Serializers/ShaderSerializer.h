#pragma once

#include <Coco/Core/Resources/Serializers/KeyValueResourceSerializer.h>

#include <Coco/Core/Types/Map.h>
#include "../Shader.h"

namespace Coco::Rendering
{
	/// @brief A serializer for shader files (*.cshader)
	class ShaderSerializer final : public KeyValueResourceSerializer
	{
	private:
		static constexpr const char* s_shaderIDVariable = "id";
		static constexpr const char* s_shaderNameVariable = "name";
		static constexpr const char* s_shaderGroupVariable = "group";
		static constexpr const char* s_subshaderSection = "subshaders";
		static constexpr const char* s_stagesSection = "stages";
		static constexpr const char* s_stageTypeVariable = "type";
		static constexpr const char* s_stageFileVariable = "file";
		static constexpr const char* s_stateSection = "state";
		static constexpr const char* s_stateTopologyModeVariable = "topologyMode";
		static constexpr const char* s_stateCullingModeVariable = "cullingMode";
		static constexpr const char* s_stateFillModeVariable = "fillMode";
		static constexpr const char* s_stateEnableDepthClampingVariable = "enableDepthClamping";
		static constexpr const char* s_stateDepthTestingModeVariable = "depthTestingMode";
		static constexpr const char* s_stateEnableDepthWriteVariable = "enableDepthWrite";
		static constexpr const char* s_attributesSection = "attributes";
		static constexpr const char* s_attributeTypeVariable = "type";
		static constexpr const char* s_descriptorsSection = "descriptors";
		static constexpr const char* s_descriptorScopeVariable = "scope";
		static constexpr const char* s_descriptorBindPointVariable = "bindPoint";
		static constexpr const char* s_descriptorTypeVariable = "type";
		static constexpr const char* s_samplersSection = "samplers";
		static constexpr const char* s_samplerScopeVariable = "scope";
		static constexpr const char* s_samplerBindPointVariable = "bindPoint";

	public:
		ShaderSerializer() = default;
		~ShaderSerializer() final = default;

		DefineSerializerResourceType(Shader)

		string Serialize(ResourceLibrary& library, const Ref<Resource>& resource) final;
		ManagedRef<Resource> Deserialize(ResourceLibrary& library, const string& data) final;

	private:
		/// @brief Reads a list of subshaders for a shader
		/// @param reader The reader
		/// @param subshaders Will be filled with subshaders
		void ReadSubshaders(KeyValueReader& reader, List<Subshader>& subshaders);

		/// @brief Reads the stages of a subshader from a file
		/// @param reader The reader
		/// @param stages Will be filled with subshader stages
		void ReadSubshaderStages(KeyValueReader& reader, List<ShaderStage>& stages);

		/// @brief Reads a GraphicsPipelineState for a subshader
		/// @param reader The reader
		/// @param state Will be filled out with the state
		void ReadSubshaderState(KeyValueReader& reader, GraphicsPipelineState& state);

		/// @brief Reads vertex attributes for a subshader
		/// @param reader The reader
		/// @param attributes Will be filled out with vertex attributes
		void ReadSubshaderAttributes(KeyValueReader& reader, List<ShaderVertexAttribute>& attributes);

		/// @brief Reads uniforms for a subshader
		/// @param reader The reader
		/// @param uniforms Will be filled out with uniforms
		void ReadSubshaderUniforms(KeyValueReader& reader, List<ShaderUniformDescriptor>& uniforms);

		/// @brief Reads texture samplers for a subshader
		/// @param reader The reader
		/// @param samplers Will be filled out with texture samplers
		void ReadSubshaderSamplers(KeyValueReader& reader, List<ShaderTextureSampler>& samplers);
	};
}