#pragma once

#include <Coco/Core/Resources/Loaders/KeyValueResourceLoader.h>

#include <Coco/Core/Types/Map.h>
#include "../ShaderTypes.h"

namespace Coco
{
	class File;
}

namespace Coco::Rendering
{
	/// @brief A loader for shader files (*.cshader)
	class ShaderLoader final : public KeyValueResourceLoader
	{
	private:
		static constexpr const char* s_shaderNameVariable = "name";
		static constexpr const char* s_subshaderSection = "subshaders";
		static constexpr const char* s_stagesSection = "stages";
		static constexpr const char* s_stateSection = "state";
		static constexpr const char* s_stateTopologyModeVariable = "topologyMode";
		static constexpr const char* s_stateCullingModeVariable = "cullingMode";
		static constexpr const char* s_stateFillModeVariable = "fillMode";
		static constexpr const char* s_stateEnableDepthClampingVariable = "enableDepthClamping";
		static constexpr const char* s_stateDepthTestingModeVariable = "depthTestingMode";
		static constexpr const char* s_stateEnableDepthWriteVariable = "enableDepthWrite";
		static constexpr const char* s_attributesSection = "attributes";
		static constexpr const char* s_descriptorsSection = "descriptors";
		static constexpr const char* s_samplersSection = "samplers";
		static constexpr const char* s_subshaderBindStageVariable = "descriptorBindStage";

	public:
		ShaderLoader(ResourceLibrary* library);
		~ShaderLoader() final = default;

		string GetResourceTypename() const noexcept final { return ResourceTypeToString(ResourceType::Shader); }

	protected:
		Ref<Resource> LoadImpl(const string& path) final;
		void SaveImpl(const Ref<Resource>& resource, const string& path) final;

	private:
		/// @brief Reads a list of subshaders for a shader
		/// @param reader The reader
		/// @param subshaders Will be filled with subshaders
		void ReadSubshaders(KeyValueReader& reader, List<Subshader>& subshaders);

		/// @brief Reads the stages of a subshader from a file
		/// @param reader The reader
		/// @param stageFiles Will be filled with subshader stage files
		void ReadSubshaderStages(KeyValueReader& reader, Map<ShaderStageType, string>& stageFiles);

		/// @brief Reads a GraphicsPipelineState for a subshader
		/// @param reader The reader
		/// @param state Will be filled out with the state
		void ReadSubshaderState(KeyValueReader& reader, GraphicsPipelineState& state);

		/// @brief Reads vertex attributes for a subshader
		/// @param reader The reader
		/// @param attributes Will be filled out with vertex attributes
		void ReadSubshaderAttributes(KeyValueReader& reader, List<ShaderVertexAttribute>& attributes);

		/// @brief Reads descriptors for a subshader
		/// @param reader The reader
		/// @param descriptors Will be filled out with descriptors
		void ReadSubshaderDescriptors(KeyValueReader& reader, List<ShaderDescriptor>& descriptors);

		/// @brief Reads texture samplers for a subshader
		/// @param reader The reader
		/// @param samplers Will be filled out with texture samplers
		void ReadSubshaderSamplers(KeyValueReader& reader, List<ShaderTextureSampler>& samplers);
	};
}