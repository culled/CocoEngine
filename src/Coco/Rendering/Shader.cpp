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
		const List<ShaderUniformDescriptor>& uniforms,
		const List<ShaderTextureSampler>& samplers)
	{
		_subshaders.Construct(name, stages, pipelineState, attributes, uniforms, samplers);
	}

	ShaderUniformData Shader::GetUniformPropertyMap() const
	{
		ShaderUniformData data{};

		for (const Subshader& subshader : _subshaders)
		{
			for (int i = 0; i < subshader.Uniforms.Count(); i++)
			{
				const ShaderUniformDescriptor& uniform = subshader.Uniforms[i];

				switch (uniform.Type)
				{
				case BufferDataFormat::Vector4:
				{
					// Skip duplicate properties
					if (data.Vector4s.contains(uniform.Name))
						continue;

					data.Vector4s[uniform.Name] = Vector4::Zero;
					break;
				}
				case BufferDataFormat::Color:
				{
					// Skip duplicate properties
					if (data.Colors.contains(uniform.Name))
						continue;

					data.Colors[uniform.Name] = Color::Black;
					break;
				}
				case BufferDataFormat::Matrix4x4:
				{
					// Skip duplicate properties
					if (data.Matrix4x4s.contains(uniform.Name))
						continue;

					data.Matrix4x4s[uniform.Name] = Matrix4x4::Identity;
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
