#include "ShaderSerializer.h"

#include <sstream>
#include <Coco/Core/Types/UUID.h>

namespace Coco::Rendering
{
	string ShaderSerializer::Serialize(ResourceLibrary& library, const Ref<Resource>& resource)
	{
		if (const Shader* shader = dynamic_cast<const Shader*>(resource.Get()))
		{
			std::stringstream stream;
			KeyValueWriter writer(stream);

			writer.WriteLine("version", "1");
			writer.WriteLine(s_shaderNameVariable, shader->GetName());

			writer.WriteLine(s_subshaderSection);
			writer.IncrementIndentLevel();

			for (const Subshader& subshader : shader->GetSubshaders())
			{
				writer.WriteLine(subshader.PassName);
				writer.IncrementIndentLevel();

				writer.WriteLine(s_stagesSection);
				writer.IncrementIndentLevel();
				for (const auto& stage : subshader.Stages)
				{
					writer.WriteLine(stage.EntryPointName);
					writer.IncrementIndentLevel();
					writer.WriteLine(s_stageTypeVariable, ToString(static_cast<int>(stage.Type)));
					writer.WriteLine(s_stageFileVariable, stage.FilePath);
					writer.DecrementIndentLevel();
				}
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
			}

			writer.Flush();
			return stream.str();
		}
		else
		{
			throw InvalidOperationException("Resource was not a shader");
		}
	}

	ManagedRef<Resource> ShaderSerializer::Deserialize(ResourceLibrary& library, const string& data)
	{
		string shaderID;
		string shaderName;
		string shaderGroup;
		List<Subshader> subshaders;

		std::stringstream stream(data);
		KeyValueReader reader(stream);

		while (reader.ReadLine())
		{
			if (reader.IsKey("version") && reader.GetValue() != "1")
				throw InvalidOperationException("Mismatching shader versions");
			else if (reader.IsKey(s_shaderIDVariable))
				shaderID = reader.GetValue();
			else if (reader.IsKey(s_shaderNameVariable))
				shaderName = reader.GetValue();
			else if (reader.IsKey(s_shaderGroupVariable))
				shaderGroup = reader.GetValue();
			else if (reader.IsKey(s_subshaderSection))
				ReadSubshaders(reader, subshaders);
		}

		if (subshaders.Count() == 0)
			throw InvalidOperationException("Shader file did not have subshaders");

		ManagedRef<Shader> shader = CreateManagedRef<Shader>(UUID(shaderID), shaderName);
		shader->SetGroupTag(shaderGroup);

		for (const Subshader& subshader : subshaders)
			shader->AddSubshader(subshader);

		return std::move(shader);
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
					ReadSubshaderStages(reader, subshader.Stages);
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

	void ShaderSerializer::ReadSubshaderStages(KeyValueReader& reader, List<ShaderStage>& stages)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			ShaderStage stage{};
			stage.EntryPointName = reader.GetKey();
			
			while (reader.ReadIfIsIndentLevel(4))
			{
				if (reader.IsKey(s_stageTypeVariable))
					stage.Type = static_cast<ShaderStageType>(reader.GetVariableValueAsInt());
				else if (reader.IsKey(s_stageFileVariable))
					stage.FilePath = reader.GetValue();
			}
			
			stages.Add(stage);
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
			attributes.Construct(static_cast<BufferDataFormat>(reader.GetKeyAsInt()));
		}
	}

	void ShaderSerializer::ReadSubshaderDescriptors(KeyValueReader& reader, List<ShaderDescriptor>& descriptors)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			descriptors.Construct(reader.GetKey(), static_cast<BufferDataFormat>(reader.GetVariableValueAsInt()));
		}
	}

	void ShaderSerializer::ReadSubshaderSamplers(KeyValueReader& reader, List<ShaderTextureSampler>& samplers)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			samplers.Construct(reader.GetKey());
		}
	}
}
