#include "ShaderTypes.h"

#include "RenderingUtilities.h"
#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Types/Matrix.h>

namespace Coco::Rendering
{
	ShaderVertexAttribute::ShaderVertexAttribute(const string& name, BufferDataFormat dataFormat) noexcept :
		Name(name), DataFormat(dataFormat), _dataOffset(0)
	{}

	ShaderDescriptor::ShaderDescriptor(const string& name, ShaderDescriptorScope scope, ShaderStageType bindPoint) noexcept :
		Name(name), Scope(scope), BindingPoints(bindPoint)
	{}

	ShaderTextureSampler::ShaderTextureSampler(const string& name, ShaderDescriptorScope scope, ShaderStageType bindPoint, DefaultTextureType defaultTexture) noexcept :
		ShaderDescriptor(name, scope, bindPoint), DefaultTexture(defaultTexture)
	{}

	ShaderUniformDescriptor::ShaderUniformDescriptor(const string & name, ShaderDescriptorScope scope, ShaderStageType bindPoint, BufferDataFormat type) noexcept :
		ShaderDescriptor(name, scope, bindPoint), Type(type)
	{}

	ShaderUniformData::ShaderUniformData() :
		ID(ResourceID::CreateV4()), 
		Version(0), 
		Preserve(false), 
		Ints{}, 
		Vector2Ints{},
		Vector3Ints{},
		Vector4Ints{},
		Floats{},
		Vector2s{},
		Vector3s{},
		Vector4s {}, 
		Matrix4x4s{}, 
		Colors{}, 
		Textures{}
	{}

	void ShaderUniformData::Merge(const ShaderUniformData& other, bool overwriteProperties)
	{
		for (const auto& ints : other.Ints)
		{
			if (!Ints.contains(ints.first) || overwriteProperties)
				Ints[ints.first] = ints.second;
		}

		for (const auto& vec2Ints : other.Vector2Ints)
		{
			if (!Vector2Ints.contains(vec2Ints.first) || overwriteProperties)
				Vector2Ints[vec2Ints.first] = vec2Ints.second;
		}

		for (const auto& vec3Ints : other.Vector3Ints)
		{
			if (!Vector3Ints.contains(vec3Ints.first) || overwriteProperties)
				Vector3Ints[vec3Ints.first] = vec3Ints.second;
		}

		for (const auto& vec4Ints : other.Vector4Ints)
		{
			if (!Vector4Ints.contains(vec4Ints.first) || overwriteProperties)
				Vector4Ints[vec4Ints.first] = vec4Ints.second;
		}

		for (const auto& floats : other.Floats)
		{
			if (!Floats.contains(floats.first) || overwriteProperties)
				Floats[floats.first] = floats.second;
		}

		for (const auto& vec2s : other.Vector2s)
		{
			if (!Vector2s.contains(vec2s.first) || overwriteProperties)
				Vector2s[vec2s.first] = vec2s.second;
		}

		for (const auto& vec3s : other.Vector3s)
		{
			if (!Vector3s.contains(vec3s.first) || overwriteProperties)
				Vector3s[vec3s.first] = vec3s.second;
		}

		for (const auto& vec4s : other.Vector4s)
		{
			if (!Vector4s.contains(vec4s.first) || overwriteProperties)
				Vector4s[vec4s.first] = vec4s.second;
		}

		for (const auto& colors : other.Colors)
		{
			if (!Colors.contains(colors.first) || overwriteProperties)
				Colors[colors.first] = colors.second;
		}

		for (const auto& mat : other.Matrix4x4s)
		{
			if (!Matrix4x4s.contains(mat.first) || overwriteProperties)
				Matrix4x4s[mat.first] = mat.second;
		}

		for (const auto& textures : other.Textures)
		{
			if (!Textures.contains(textures.first) || overwriteProperties)
				Textures[textures.first] = textures.second;
		}
	}

	void ShaderUniformData::CopyFrom(const ShaderUniformData& other)
	{
		for (auto& ints : Ints)
		{
			if (other.Ints.contains(ints.first))
				ints.second = other.Ints.at(ints.first);
		}

		for (auto& vec2Int : Vector2Ints)
		{
			if (other.Vector2Ints.contains(vec2Int.first))
				vec2Int.second = other.Vector2Ints.at(vec2Int.first);
		}

		for (auto& vec3Int : Vector3Ints)
		{
			if (other.Vector3Ints.contains(vec3Int.first))
				vec3Int.second = other.Vector3Ints.at(vec3Int.first);
		}

		for (auto& vec4Int : Vector4Ints)
		{
			if (other.Vector4Ints.contains(vec4Int.first))
				vec4Int.second = other.Vector4Ints.at(vec4Int.first);
		}

		for (auto& floats : Floats)
		{
			if (other.Floats.contains(floats.first))
				floats.second = other.Floats.at(floats.first);
		}

		for (auto& vec2 : Vector2s)
		{
			if (other.Vector2s.contains(vec2.first))
				vec2.second = other.Vector2s.at(vec2.first);
		}

		for (auto& vec3 : Vector3s)
		{
			if (other.Vector3s.contains(vec3.first))
				vec3.second = other.Vector3s.at(vec3.first);
		}

		for (auto& vec4 : Vector4s)
		{
			if (other.Vector4s.contains(vec4.first))
				vec4.second = other.Vector4s.at(vec4.first);
		}

		for (auto& color : Colors)
		{
			if (other.Colors.contains(color.first))
				color.second = other.Colors.at(color.first);
		}

		for (auto& mat : Matrix4x4s)
		{
			if (other.Matrix4x4s.contains(mat.first))
				mat.second = other.Matrix4x4s.at(mat.first);
		}

		for (auto& texture : Textures)
		{
			if (other.Textures.contains(texture.first))
				texture.second = other.Textures.at(texture.first);
		}
	}

	ShaderStage::ShaderStage() : EntryPointName(), Type(ShaderStageType::None), FilePath()
	{}

	ShaderStage::ShaderStage(const string& entryPointName, ShaderStageType stageType, const string& filePath) :
		EntryPointName(entryPointName), Type(stageType), FilePath(filePath)
	{}

	Subshader::Subshader(
		const string& name,
		const List<ShaderStage>& stages,
		const GraphicsPipelineState& pipelineState,
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderUniformDescriptor>& uniforms,
		const List<ShaderTextureSampler>& samplers) noexcept :
		PassName(name),
		Stages(stages),
		PipelineState(pipelineState),
		Attributes(attributes),
		Uniforms(uniforms),
		Samplers(samplers)
	{
		UpdateAttributeOffsets();
	}

	List<ShaderUniformDescriptor> Subshader::GetScopedUniforms(ShaderDescriptorScope scope) const
	{
		List<ShaderUniformDescriptor> uniforms;

		for (const auto& uniform : Uniforms)
		{
			if (uniform.Scope == scope)
				uniforms.Add(uniform);
		}

		return uniforms;
	}

	List<ShaderTextureSampler> Subshader::GetScopedSamplers(ShaderDescriptorScope scope) const
	{
		List<ShaderTextureSampler> samplers;

		for (const ShaderTextureSampler& sampler : Samplers)
		{
			if (sampler.Scope == scope)
				samplers.Add(sampler);
		}

		return samplers;
	}

	bool Subshader::HasScope(ShaderDescriptorScope scope) const
	{
		for (const ShaderUniformDescriptor& uniform : Uniforms)
		{
			if (uniform.Scope == scope)
				return true;
		}

		for (const ShaderTextureSampler& sampler : Samplers)
		{
			if (sampler.Scope == scope)
				return true;
		}

		return false;
	}

	ShaderStageType Subshader::GetUniformBindingStages(ShaderDescriptorScope scope) const
	{
		ShaderStageType bindPoint = ShaderStageType::None;

		for (const auto& uniform : Uniforms)
		{
			if (uniform.Scope == scope && uniform.BindingPoints < bindPoint)
				bindPoint |= uniform.BindingPoints;
		}

		return bindPoint;
	}

	uint32_t Subshader::GetVertexDataSize() const
	{
		uint32_t size = 0;

		for (const auto& attr : Attributes)
		{
			size += GetBufferDataFormatSize(attr.DataFormat);
		}

		return size;
	}

	void Subshader::UpdateAttributeOffsets() noexcept
	{
		uint32_t offset = 0;

		for (ShaderVertexAttribute& attr : Attributes)
		{
			attr._dataOffset = offset;
			offset += GetBufferDataFormatSize(attr.DataFormat);
		}
	}
}