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

	List<char> Subshader::GetUniformData(ShaderDescriptorScope scope, const ShaderUniformData& data, uint minimumAlignment) const
	{
		const uint64_t alignedFloatSize = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Float), minimumAlignment);
		const uint64_t alignedVec2Size = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector2), minimumAlignment);
		const uint64_t alignedVec3Size = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector3), minimumAlignment);
		const uint64_t alignedVec4Size = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector4), minimumAlignment);
		const uint64_t alignedIntSize = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Int), minimumAlignment);
		const uint64_t alignedVec2IntSize = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector2Int), minimumAlignment);
		const uint64_t alignedVec3IntSize = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector3Int), minimumAlignment);
		const uint64_t alignedVec4IntSize = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector4Int), minimumAlignment);
		const uint64_t alignedMat4Size = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Matrix4x4), minimumAlignment);

		Array<float, 16> tempData = { 0.0f };
		Array<int32_t, 4> tempIntData = { 0 };

		List<char> uniformData;
		uint64_t offset = 0;
		List<ShaderUniformDescriptor> uniforms = GetScopedUniforms(scope);

		for (int i = 0; i < uniforms.Count(); i++)
		{
			const ShaderUniformDescriptor& uniform = uniforms[i];

			switch (uniform.Type)
			{
			case BufferDataFormat::Int:
			{
				int32_t v = 0;

				if (data.Ints.contains(uniform.Name))
					v = data.Ints.at(uniform.Name);

				uniformData.Resize(uniformData.Count() + alignedIntSize);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, &v, sizeof(int32_t));

				offset += alignedIntSize;
				break;
			}
			case BufferDataFormat::Vector2Int:
			{
				Vector2Int vec2;

				if (data.Vector2Ints.contains(uniform.Name))
					vec2 = data.Vector2Ints.at(uniform.Name);

				tempIntData[0] = static_cast<int32_t>(vec2.X);
				tempIntData[1] = static_cast<int32_t>(vec2.Y);

				uniformData.Resize(uniformData.Count() + alignedVec2IntSize);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempIntData.data(), 2 * sizeof(int32_t));

				offset += alignedVec2IntSize;
				break;
			}
			case BufferDataFormat::Vector3Int:
			{
				Vector3Int vec3;

				if (data.Vector3Ints.contains(uniform.Name))
					vec3 = data.Vector3Ints.at(uniform.Name);

				tempIntData[0] = static_cast<int32_t>(vec3.X);
				tempIntData[1] = static_cast<int32_t>(vec3.Y);
				tempIntData[2] = static_cast<int32_t>(vec3.Z);

				uniformData.Resize(uniformData.Count() + alignedVec3IntSize);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempIntData.data(), 3 * sizeof(int32_t));

				offset += alignedVec3IntSize;
				break;
			}
			case BufferDataFormat::Vector4Int:
			{
				Vector4Int vec4;

				if (data.Vector4Ints.contains(uniform.Name))
					vec4 = data.Vector4Ints.at(uniform.Name);

				tempIntData[0] = static_cast<int32_t>(vec4.X);
				tempIntData[1] = static_cast<int32_t>(vec4.Y);
				tempIntData[2] = static_cast<int32_t>(vec4.Z);
				tempIntData[3] = static_cast<int32_t>(vec4.W);

				uniformData.Resize(uniformData.Count() + alignedVec4IntSize);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempIntData.data(), 4 * sizeof(int32_t));

				offset += alignedVec4IntSize;
				break;
			}
			case BufferDataFormat::Float:
			{
				float v = 0.0f;

				if (data.Floats.contains(uniform.Name))
					v = data.Floats.at(uniform.Name);

				uniformData.Resize(uniformData.Count() + alignedFloatSize);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, &v, sizeof(float));

				offset += alignedFloatSize;
				break;
			}
			case BufferDataFormat::Vector2:
			{
				Vector2 vec2;

				if (data.Vector2s.contains(uniform.Name))
					vec2 = data.Vector2s.at(uniform.Name);

				tempData[0] = static_cast<float>(vec2.X);
				tempData[1] = static_cast<float>(vec2.Y);

				uniformData.Resize(uniformData.Count() + alignedVec2Size);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempData.data(), 2 * sizeof(float));

				offset += alignedVec2Size;
				break;
			}
			case BufferDataFormat::Vector3:
			{
				Vector3 vec3;

				if (data.Vector3s.contains(uniform.Name))
					vec3 = data.Vector3s.at(uniform.Name);

				tempData[0] = static_cast<float>(vec3.X);
				tempData[1] = static_cast<float>(vec3.Y);
				tempData[2] = static_cast<float>(vec3.Z);

				uniformData.Resize(uniformData.Count() + alignedVec3Size);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempData.data(), 3 * sizeof(float));

				offset += alignedVec3Size;
				break;
			}
			case BufferDataFormat::Vector4:
			{
				Vector4 vec4;

				if (data.Vector4s.contains(uniform.Name))
					vec4 = data.Vector4s.at(uniform.Name);

				tempData[0] = static_cast<float>(vec4.X);
				tempData[1] = static_cast<float>(vec4.Y);
				tempData[2] = static_cast<float>(vec4.Z);
				tempData[3] = static_cast<float>(vec4.W);

				uniformData.Resize(uniformData.Count() + alignedVec4Size);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempData.data(), 4 * sizeof(float));

				offset += alignedVec4Size;
				break;
			}
			case BufferDataFormat::Color:
			{
				Color c;

				if (data.Colors.contains(uniform.Name))
					c = data.Colors.at(uniform.Name).AsLinear();

				tempData[0] = static_cast<float>(c.R);
				tempData[1] = static_cast<float>(c.G);
				tempData[2] = static_cast<float>(c.B);
				tempData[3] = static_cast<float>(c.A);

				uniformData.Resize(uniformData.Count() + alignedVec4Size);

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempData.data(), 4 * sizeof(float));

				offset += alignedVec4Size;
				break;
			}
			case BufferDataFormat::Matrix4x4:
			{
				Matrix4x4 mat4;

				if (data.Matrix4x4s.contains(uniform.Name))
					mat4 = data.Matrix4x4s.at(uniform.Name);

				uniformData.Resize(uniformData.Count() + alignedMat4Size);

				tempData = mat4.AsFloat();

				char* dst = (uniformData.Data() + offset);
				std::memcpy(dst, tempData.data(), 16 * sizeof(float));

				offset += alignedMat4Size;
				break;
			}
			default:
				break;
			}
		}

		return uniformData;
	}

	uint64_t Subshader::GetUniformDataSize(ShaderDescriptorScope scope, uint minimumAlignment) const
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