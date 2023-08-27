#include "ShaderTypes.h"

#include "RenderingUtilities.h"
#include <Coco/Core/Types/Array.h>
#include <Coco/Core/Types/Matrix.h>

namespace Coco::Rendering
{
	ShaderVertexAttribute::ShaderVertexAttribute(BufferDataFormat dataFormat) noexcept :
		DataFormat(dataFormat), _dataOffset(0)
	{}

	ShaderDescriptor::ShaderDescriptor(const string& name, ShaderDescriptorScope scope, ShaderStageType bindPoint) noexcept :
		Name(name), Scope(scope), BindingPoints(bindPoint)
	{}

	ShaderTextureSampler::ShaderTextureSampler(const string& name, ShaderDescriptorScope scope, ShaderStageType bindPoint) noexcept :
		ShaderDescriptor(name, scope, bindPoint)
	{}

	ShaderUniformDescriptor::ShaderUniformDescriptor(const string & name, ShaderDescriptorScope scope, ShaderStageType bindPoint, BufferDataFormat type) noexcept :
		ShaderDescriptor(name, scope, bindPoint), Type(type)
	{}

	ShaderUniformData::ShaderUniformData() :
		ID(ResourceID::CreateV4()), Version(0), Preserve(false), Vector4s{}, Matrix4x4s{}, Colors {}, Textures{}
	{}

	void ShaderUniformData::Merge(const ShaderUniformData& other, bool overwriteProperties)
	{
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

	List<char> Subshader::GetUniformData(ShaderDescriptorScope scope, const ShaderUniformData& data, uint minimumAlignment) const
	{
		const uint64_t alignedVec4Size = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector4), minimumAlignment);
		const uint64_t alignedMat4Size = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Matrix4x4), minimumAlignment);
		Array<float, 4> tempVec4 = { 0.0f };
		Array<float, 16> tempMat4 = { 0.0f };

		List<char> uniformData;
		uint64_t offset = 0;
		List<ShaderUniformDescriptor> uniforms = GetScopedUniforms(scope);

		for (int i = 0; i < uniforms.Count(); i++)
		{
			const ShaderUniformDescriptor& uniform = uniforms[i];

			switch (uniform.Type)
			{
			case BufferDataFormat::Vector4:
			{
				Vector4 vec4;

				if (data.Vector4s.contains(uniform.Name))
					vec4 = data.Vector4s.at(uniform.Name);

				tempVec4[0] = static_cast<float>(vec4.X);
				tempVec4[1] = static_cast<float>(vec4.Y);
				tempVec4[2] = static_cast<float>(vec4.Z);
				tempVec4[3] = static_cast<float>(vec4.W);

				uniformData.Resize(uniformData.Count() + alignedVec4Size);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempVec4.data(), tempVec4.size() * sizeof(float));

				offset += alignedVec4Size;
				break;
			}
			case BufferDataFormat::Color:
			{
				Color c;

				if (data.Colors.contains(uniform.Name))
					c = data.Colors.at(uniform.Name).AsLinear();

				tempVec4[0] = static_cast<float>(c.R);
				tempVec4[1] = static_cast<float>(c.G);
				tempVec4[2] = static_cast<float>(c.B);
				tempVec4[3] = static_cast<float>(c.A);

				uniformData.Resize(uniformData.Count() + alignedVec4Size);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempVec4.data(), tempVec4.size() * sizeof(float));

				offset += alignedVec4Size;
				break;
			}
			case BufferDataFormat::Matrix4x4:
			{
				Matrix4x4 mat4;

				if (data.Matrix4x4s.contains(uniform.Name))
					mat4 = data.Matrix4x4s.at(uniform.Name);

				uniformData.Resize(uniformData.Count() + alignedMat4Size);

				tempMat4 = mat4.AsFloat();

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempMat4.data(), tempMat4.size() * sizeof(float));

				offset += alignedMat4Size;
				break;
			}
			default:
				break;
			}
		}

		return uniformData;
	}

	uint64_t Subshader::GetDescriptorDataSize(ShaderDescriptorScope scope, uint minimumAlignment) const
	{
		uint64_t size = 0;
		List<ShaderUniformDescriptor> uniforms = GetScopedUniforms(scope);

		for (const auto& uniform : uniforms)
		{
			size += RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(uniform.Type), minimumAlignment);
		}

		return size;
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