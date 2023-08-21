#include "ShaderTypes.h"

#include "RenderingUtilities.h"
#include <Coco/Core/Types/Array.h>

namespace Coco::Rendering
{
	ShaderVertexAttribute::ShaderVertexAttribute(BufferDataFormat dataFormat) noexcept :
		DataFormat(dataFormat), _dataOffset(0)
	{}

	ShaderDescriptor::ShaderDescriptor(const string& name, BufferDataFormat type) noexcept :
		Name(name), Type(type)
	{}

	ShaderTextureSampler::ShaderTextureSampler(const string& name) noexcept :
		Name(name)
	{}

	Subshader::Subshader(
		const string& name,
		const UnorderedMap<ShaderStageType, string>& stageFiles,
		const GraphicsPipelineState& pipelineState,
		const List<ShaderVertexAttribute>& attributes,
		const List<ShaderDescriptor>& descriptors,
		const List<ShaderTextureSampler>& samplers,
		ShaderStageType bindPoint) noexcept :
		PassName(name),
		StageFiles(stageFiles),
		PipelineState(pipelineState),
		Attributes(attributes),
		Descriptors(descriptors),
		Samplers(samplers),
		DescriptorBindingPoint(bindPoint)
	{
		UpdateAttributeOffsets();
	}

	List<char> Subshader::GetUniformData(const ShaderUniformData& data, uint minimumAlignment) const
	{
		const uint64_t alignedVec4Size = RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(BufferDataFormat::Vector4), minimumAlignment);
		Array<float, 4> tempVec4 = { 0.0f };

		List<char> uniformData;
		uint64_t offset = 0;

		for (int i = 0; i < Descriptors.Count(); i++)
		{
			const ShaderDescriptor& descriptor = Descriptors[i];

			switch (descriptor.Type)
			{
			case BufferDataFormat::Vector4:
			{
				Vector4 vec4;

				if (data.Vector4s.contains(descriptor.Name))
					vec4 = data.Vector4s.at(descriptor.Name);

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

				if (data.Colors.contains(descriptor.Name))
					c = data.Colors.at(descriptor.Name).AsLinear();

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
			default:
				break;
			}
		}

		return uniformData;
	}

	uint64_t Subshader::GetDescriptorDataSize(uint minimumAlignment) const
	{
		uint64_t size = 0;

		for (const auto& descriptor : Descriptors)
		{
			size += RenderingUtilities::GetOffsetForAlignment(GetBufferDataFormatSize(descriptor.Type), minimumAlignment);
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

		for (auto& texture : Textures)
		{
			if (other.Textures.contains(texture.first))
				texture.second = other.Textures.at(texture.first);
		}
	}
}