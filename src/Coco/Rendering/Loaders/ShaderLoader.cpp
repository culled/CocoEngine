#include "ShaderLoader.h"

#include <Coco/Core/IO/File.h>
#include "../Shader.h"

namespace Coco::Rendering
{
	ShaderLoader::ShaderLoader(ResourceLibrary* library) : KeyValueResourceLoader(library)
	{}

	Ref<Resource> ShaderLoader::LoadImpl(const string& path)
	{
		string shaderName;
		List<Subshader> subshaders;

		File file = File::Open(path, FileModeFlags::Read);
		KeyValueReader reader(file);
		while (reader.ReadLine())
		{
			if (reader.IsKey("version") && reader.GetValue() != "1")
				throw InvalidOperationException("Mismatching shader versions");
			else if (reader.IsKey(s_shaderNameVariable))
				shaderName = reader.GetValue();
			else if (reader.IsKey(s_subshaderSection))
				ReadSubshaders(reader, subshaders);
		}

		file.Close();

		if (subshaders.Count() == 0)
			throw InvalidOperationException("Shader file did not have subshaders");

		Ref<Shader> shader = CreateRef<Shader>(shaderName);

		for (const Subshader& subshader : subshaders)
			shader->AddSubshader(subshader);

		return shader;
	}

	void ShaderLoader::SaveImpl(const Ref<Resource>& resource, const string& path)
	{
		if (Shader* shader = dynamic_cast<Shader*>(resource.get()))
		{
			File file = File::Open(path, FileModeFlags::Write);
			KeyValueWriter writer(file);

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
			}

			file.Close();
		}
		else
		{
			throw InvalidOperationException("Resource was not a shader");
		}
	}

	void ShaderLoader::ReadSubshaders(KeyValueReader& reader, List<Subshader>& subshaders)
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

	void ShaderLoader::ReadSubshaderStages(KeyValueReader& reader, Map<ShaderStageType, string>& stageFiles)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			ShaderStageType stageType = static_cast<ShaderStageType>(reader.GetKeyAsInt());
			stageFiles[stageType] = reader.GetValue();
		}
	}

	void ShaderLoader::ReadSubshaderState(KeyValueReader& reader, GraphicsPipelineState& state)
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

	void ShaderLoader::ReadSubshaderAttributes(KeyValueReader& reader, List<ShaderVertexAttribute>& attributes)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			attributes.Add(ShaderVertexAttribute(static_cast<BufferDataFormat>(reader.GetKeyAsInt())));
		}
	}

	void ShaderLoader::ReadSubshaderDescriptors(KeyValueReader& reader, List<ShaderDescriptor>& descriptors)
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

	void ShaderLoader::ReadSubshaderSamplers(KeyValueReader& reader, List<ShaderTextureSampler>& samplers)
	{
		while (reader.ReadIfIsIndentLevel(3))
		{
			samplers.Add(ShaderTextureSampler(reader.GetKey()));
		}
	}
}
