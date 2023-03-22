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
	/// <summary>
	/// A loader for shader files (*.cshader)
	/// </summary>
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
		ShaderLoader(const string& basePath);
		~ShaderLoader() final = default;

		const char* GetResourceTypename() const noexcept final { return ResourceTypeToString(ResourceType::Shader); }

	protected:
		Ref<Resource> LoadImpl(const string& path) final;
		void SaveImpl(const Ref<Resource>& resource, const string& path) final;

	private:
		/// <summary>
		/// Reads a list of subshaders from a file
		/// </summary>
		/// <param name="reader">The reader (should be positioned after the start of the "subshaders" section</param>
		/// <param name="subshaders">A list of subshaders that will be filled out</param>
		void ReadSubshaders(KeyValueReader& reader, List<Subshader>& subshaders);

		/// <summary>
		/// Reads the stages of a subshader from a file
		/// </summary>
		/// <param name="reader">The reader (should be positioned after the start of the "stages" section</param>
		/// <param name="stageFiles">A map of subshader stages that will be filled out</param>
		void ReadSubshaderStages(KeyValueReader& reader, Map<ShaderStageType, string>& stageFiles);

		/// <summary>
		/// Reads a GraphicsPipelineState for a subshader
		/// </summary>
		/// <param name="reader">The reader (should be positioned after the start of the "state" section</param>
		/// <param name="state">The state that will be filled out</param>
		void ReadSubshaderState(KeyValueReader& reader, GraphicsPipelineState& state);

		/// <summary>
		/// Reads vertex attributes for a subshader
		/// </summary>
		/// <param name="reader">The reader (should be positioned after the start of the "attributes" section</param>
		/// <param name="attributes">The vertex attributes that will be filled out</param>
		void ReadSubshaderAttributes(KeyValueReader& reader, List<ShaderVertexAttribute>& attributes);

		/// <summary>
		/// Reads vertex descriptors for a subshader
		/// </summary>
		/// <param name="reader">The reader (should be positioned after the start of the "descriptors" section</param>
		/// <param name="descriptors">The descriptors that will be filled out</param>
		void ReadSubshaderDescriptors(KeyValueReader& reader, List<ShaderDescriptor>& descriptors);

		/// <summary>
		/// Reads texture samplers for a subshader
		/// </summary>
		/// <param name="reader">The reader (should be positioned after the start of the "samplers" section</param>
		/// <param name="samplers">The samplers that will be filled out</param>
		void ReadSubshaderSamplers(KeyValueReader& reader, List<ShaderTextureSampler>& samplers);
	};
}