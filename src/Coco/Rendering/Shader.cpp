#include "Shader.h"

namespace Coco::Rendering
{
	Shader::Shader(const ResourceID& id, const string& name) : RenderingResource(id, name)
	{}

	bool Shader::TryGetSubshader(const string& name, const Subshader*& subshader) const noexcept
	{
		for (const Subshader& sub : _subshaders)
		{
			if (sub.PassName == name)
			{
				subshader = &sub;
				return true;
			}
		}

		return false;
	}

	void Shader::AddSubshader(const Subshader& subshader)
	{
		_subshaders.Add(subshader);
		_subshaders.Last().UpdateAttributeOffsets();
	}

	void Shader::CreateSubshader(
		const string& name, 
		const List<ShaderStage>& stages,
		const GraphicsPipelineState& pipelineState, 
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderDescriptor>& descriptors,
		const List<ShaderTextureSampler>& samplers,
		ShaderStageType bindPoint)
	{
		_subshaders.Construct(name, stages, pipelineState, attributes, descriptors, samplers, bindPoint);
	}

	ShaderUniformData Shader::GetUniformPropertyMap() const
	{
		ShaderUniformData data{};

		for (const Subshader& subshader : _subshaders)
		{
			for (int i = 0; i < subshader.Descriptors.Count(); i++)
			{
				const ShaderDescriptor& descriptor = subshader.Descriptors[i];

				switch (descriptor.Type)
				{
				case BufferDataFormat::Vector4:
				{
					// Skip duplicate properties
					if (data.Vector4s.contains(descriptor.Name))
						continue;

					data.Vector4s[descriptor.Name] = Vector4::Zero;
					break;
				}
				case BufferDataFormat::Color:
				{
					// Skip duplicate properties
					if (data.Colors.contains(descriptor.Name))
						continue;

					data.Colors[descriptor.Name] = Color::Black;
					break;
				}
				default:
					break;
				}
			}

			for (int i = 0; i < subshader.Samplers.Count(); i++)
			{
				const ShaderTextureSampler& sampler = subshader.Samplers[i];

				// Skip duplicate properties
				if (data.Textures.contains(sampler.Name))
					continue;

				data.Textures[sampler.Name] = Resource::InvalidID;
				break;
			}
		}

		return data;
	}
}
