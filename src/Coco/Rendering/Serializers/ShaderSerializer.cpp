#include "ShaderSerializer.h"

#include <sstream>

namespace Coco::Rendering
{
	string ShaderSerializer::Serialize(ResourceLibrary& library, const Ref<Resource>& resource)
	{
		if (const Shader* shader = dynamic_cast<const Shader*>(resource.Get()))
		{
			/*KeyValueWriter writer;

			writer.WriteLine("version", "1");
			writer.WriteLine(s_shaderNameVariable, shader->Name);

			writer.WriteLine(s_subshaderSection);
			writer.IncrementIndentLevel();

			for (const Subshader& subshader : shader->GetSubshaders())
			{
				writer.WriteLine(subshader.PassName);
				writer.IncrementIndentLevel();

				writer.WriteLine(s_stagesSection);
				writer.IncrementIndentLevel();
				for (const auto& kvp : subshader.StageFiles)
					writer.WriteLine(ToString(static_cast<int>(kvp.first)), kvp.second);
				writer.DecrementIndentLevel();

				writer.WriteLine(s_stateSection);
				writer.IncrementIndentLevel();
				writer.WriteLine(s_stateTopologyModeVariable, ToString(static_cast<int>(subshader.PipelineState.TopologyMode)));
				writer.WriteLine(s_stateCullingModeVariable, ToString(static_cast<int>(subshader.PipelineState.CullingMode)));
				writer.WriteLine(s_stateFillModeVariable, ToString(static_cast<int>(subshader.PipelineState.PolygonFillMode)));
				writer.WriteLine(s_stateEnableDepthClampingVariable, ToString(subshader.PipelineState.EnableDepthClamping));
				writer.WriteLine(s_stateDepthTestingModeVariable, ToString(static_cast<int>(subshader.PipelineState.DepthTestingMode)));
				writer.WriteLine(s_stateEnableDepthWriteVariable, ToString(subshader.PipelineState.EnableDepthWrite));
				writer.DecrementIndentLevel();

				writer.WriteLine(s_attributesSection);
				writer.IncrementIndentLevel();
				for (const ShaderVertexAttribute& attr : subshader.Attributes)
					writer.WriteLine(ToString(static_cast<int>(attr.DataFormat)));
				writer.DecrementIndentLevel();

				writer.WriteLine(s_descriptorsSection);
				writer.IncrementIndentLevel();
				for (const ShaderDescriptor& desc : subshader.Descriptors)
					writer.WriteLine(desc.Name, ToString(static_cast<int>(desc.Type)));
				writer.DecrementIndentLevel();

				writer.WriteLine(s_samplersSection);
				writer.IncrementIndentLevel();
				for (const ShaderTextureSampler& sampler : subshader.Samplers)
					writer.WriteLine(sampler.Name);
				writer.DecrementIndentLevel();

				writer.WriteLine(s_subshaderBindStageVariable, ToString(static_cast<int>(subshader.DescriptorBindingPoint)));

				writer.SetIndentLevel(1);
			}*/

			return "";
		}
		else
		{
			throw InvalidOperationException("Resource was not a shader");
		}
	}

	void ShaderSerializer::Deserialize(ResourceLibrary& library, const string& data, Ref<Resource> resource)
	{
		string shaderName;
		List<Subshader> subshaders;

		std::stringstream stream(data);
		KeyValueReader reader(stream);

		while (reader.ReadLine())
		{
			if (reader.IsKey("version") && reader.GetValue() != "1")
				throw InvalidOperationException("Mismatching shader versions");
			else if (reader.IsKey(s_shaderNameVariable))
				shaderName = reader.GetValue();
			else if (reader.IsKey(s_subshaderSection))
				ReadSubshaders(reader, subshaders);
		}

		if (subshaders.Count() == 0)
			throw InvalidOperationException("Shader file did not have subshaders");

		Shader* shader = static_cast<Shader*>(resource.Get());
		shader->SetName(shaderName);

		for (const Subshader& subshader : subshaders)
			shader->AddSubshader(subshader);
	}

	void ShaderSerializer::ReadSubshaders(KeyValueReader& reader, List<Subshader>& subshaders)
	{
		// The file position should be at a subshader name
		while (reader.ReadIfIsIndentLevel(1))
		{
			Subshader subshader = {};
			subshader.PassName = reader.GetKey();

			while (reader.ReadIfIsIndentLevel(2))
			{
				if (reader.IsKey(s_stagesSection))
					ReadSubshaderStages(reader, subshader.StageFiles);
				else if (reader.IsKey(s_stateSection))
					ReadSubshaderState(reader, subshader.PipelineState);
				else if (reader.IsKey(s_attributesSection))
					ReadSubshaderAttributes(reader, subshader.Attributes);
				else if (reader.IsKey(s_descriptorsSection))
					ReadSubshaderDescriptors(reader, subshader.Descriptors);
				else if (reader.IsKey(s_samplersSection))
					ReadSubshaderSamplers(reader, subshader.Samplers);
				else if (reader.IsKey(s_subshaderBindStageVariable))
					subshader.DescriptorBindingPoint = static_cast<ShaderStageType>(reader.GetVariableValueAsInt());
			}

			subshaders.Add(subshader);
		}
	}

	void ShaderSerializer::ReadSubshaderStages(KeyValueReader& reader, UnorderedMap<ShaderStageType, string>& stageFiles)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			ShaderStageType stageType = static_cast<ShaderStageType>(reader.GetKeyAsInt());
			stageFiles[stageType] = reader.GetValue();
		}
	}

	void ShaderSerializer::ReadSubshaderState(KeyValueReader& reader, GraphicsPipelineState& state)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			if (reader.IsKey(s_stateTopologyModeVariable))
				state.TopologyMode = static_cast<TopologyMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_stateCullingModeVariable))
				state.CullingMode = static_cast<CullMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_stateFillModeVariable))
				state.PolygonFillMode = static_cast<PolygonFillMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_stateEnableDepthClampingVariable))
				state.EnableDepthClamping = reader.GetVariableValueAsBool();
			else if (reader.IsKey(s_stateDepthTestingModeVariable))
				state.DepthTestingMode = static_cast<DepthTestingMode>(reader.GetVariableValueAsInt());
			else if (reader.IsKey(s_stateEnableDepthWriteVariable))
				state.EnableDepthWrite = reader.GetVariableValueAsBool();
		}
	}

	void ShaderSerializer::ReadSubshaderAttributes(KeyValueReader& reader, List<ShaderVertexAttribute>& attributes)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			attributes.Add(ShaderVertexAttribute(static_cast<BufferDataFormat>(reader.GetKeyAsInt())));
		}
	}

	void ShaderSerializer::ReadSubshaderDescriptors(KeyValueReader& reader, List<ShaderDescriptor>& descriptors)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			descriptors.Add(
				ShaderDescriptor(
					reader.GetKey(), 
					static_cast<BufferDataFormat>(reader.GetVariableValueAsInt())
				)
			);
		}
	}

	void ShaderSerializer::ReadSubshaderSamplers(KeyValueReader& reader, List<ShaderTextureSampler>& samplers)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			samplers.Add(ShaderTextureSampler(reader.GetKey()));
		}
	}
}
