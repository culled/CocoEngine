#include "Renderpch.h"
#include "ShaderUniformLayout.h"
#include "GraphicsDevice.h"
#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	const uint64 ShaderUniformLayout::EmptyHash = 0;

	ShaderUniformLayout::ShaderUniformLayout() :
		DataUniforms(),
		TextureUniforms(),
		Hash(EmptyHash)
	{}

	ShaderUniformLayout::ShaderUniformLayout(const std::vector<ShaderDataUniform>& dataUniforms, const std::vector<ShaderTextureUniform>& textureUniforms) :
		DataUniforms(dataUniforms),
		TextureUniforms(textureUniforms)
	{
		this->CalculateHash();
	}

	bool ShaderUniformLayout::operator==(const ShaderUniformLayout& other) const
	{
		return Hash == other.Hash;
	}

	void ShaderUniformLayout::CalculateHash()
	{
		uint64 dataUniformHash = 0;
		for (const ShaderDataUniform& u : DataUniforms)
		{
			dataUniformHash = Math::CombineHashes(
				dataUniformHash,
				u.Key,
				static_cast<uint64>(u.BindingPoints),
				static_cast<uint64>(u.Type)
			);
		}

		uint64 textureUniformHash = 0;
		for (const ShaderTextureUniform& u : TextureUniforms)
		{
			textureUniformHash = Math::CombineHashes(
				textureUniformHash,
				u.Key,
				static_cast<uint64>(u.BindingPoints),
				static_cast<uint64>(u.DefaultTexture)
			);
		}

		Hash = Math::CombineHashes(dataUniformHash, textureUniformHash);
	}

	ShaderStageFlags ShaderUniformLayout::GetAllBindStages() const
	{
		return GetDataUniformBindStages() | GetTextureUniformBindStages();
	}

	ShaderStageFlags ShaderUniformLayout::GetDataUniformBindStages() const
	{
		ShaderStageFlags flags = ShaderStageFlags::None;

		for (const ShaderDataUniform& u : DataUniforms)
			flags |= u.BindingPoints;

		return flags;
	}

	ShaderStageFlags ShaderUniformLayout::GetTextureUniformBindStages() const
	{
		ShaderStageFlags flags = ShaderStageFlags::None;

		for (const ShaderTextureUniform& u : TextureUniforms)
			flags |= u.BindingPoints;

		return flags;
	}

	uint64 ShaderUniformLayout::GetUniformDataSize(const GraphicsDevice& device) const
	{
		uint64 offset = 0;

		for (const auto& u : DataUniforms)
		{
			device.AlignOffset(u.Type, offset);
			offset += GetDataTypeSize(u.Type);
		}

		// Pad out the data size so they fill a block accessible by the minimum buffer alignment
		offset = GraphicsDevice::GetOffsetForAlignment(offset, device.GetFeatures().MinimumBufferAlignment);

		return offset;
	}

	std::vector<uint8> ShaderUniformLayout::GetBufferFriendlyData(const GraphicsDevice& device, const ShaderUniformData& data) const
	{
		std::vector<uint8> bufferData;
		
		// TODO: better way to estimate data size?
		bufferData.reserve(DataUniforms.size() * BufferIntSize * 32);

		uint64 offset = 0;

		for (const ShaderDataUniform& uniform : DataUniforms)
		{
			const uint8 dataSize = GetDataTypeSize(uniform.Type);
			uint64 preAlignOffset = offset;
			device.AlignOffset(uniform.Type, offset);

			bufferData.resize(offset + dataSize);
			uint8* dst = bufferData.data() + offset;

			switch (uniform.Type)
			{
			case BufferDataType::Float:
			{
				if (data.Floats.contains(uniform.Key))
				{
					float d = data.Floats.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, &d, dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Float2:
			{
				if (data.Float2s.contains(uniform.Key))
				{
					const ShaderUniformData::float2& d = data.Float2s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Float3:
			{
				if (data.Float3s.contains(uniform.Key))
				{
					const ShaderUniformData::float3& d = data.Float3s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Float4:
			{
				if (data.Float4s.contains(uniform.Key))
				{
					const ShaderUniformData::float4& d = data.Float4s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Mat4x4:
			{
				if (data.Mat4x4s.contains(uniform.Key))
				{
					const ShaderUniformData::Mat4x4& d = data.Mat4x4s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int:
			{
				if (data.Ints.contains(uniform.Key))
				{
					int32 d = data.Ints.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, &d, dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int2:
			{
				if (data.Int2s.contains(uniform.Key))
				{
					const ShaderUniformData::int2& d = data.Int2s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int3:
			{
				if (data.Int3s.contains(uniform.Key))
				{
					const ShaderUniformData::int3& d = data.Int3s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Int4:
			{
				if (data.Int4s.contains(uniform.Key))
				{
					const ShaderUniformData::int4& d = data.Int4s.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, d.data(), dataSize) == 0)
				}

				break;
			}
			case BufferDataType::Bool:
			{
				if (data.Bools.contains(uniform.Key))
				{
					uint8 d = data.Bools.at(uniform.Key);
					Assert(memcpy_s(dst, dataSize, &d, dataSize) == 0)
				}

				break;
			}
			}

			offset += dataSize;
		}

		// Pad the buffer to fit in the minimum buffer alignment
		bufferData.resize(GraphicsDevice::GetOffsetForAlignment(bufferData.size(), device.GetFeatures().MinimumBufferAlignment));

		return bufferData;
	}

	GlobalShaderUniformLayout::GlobalShaderUniformLayout() :
		ShaderUniformLayout(),
		BufferUniforms()
	{}

	GlobalShaderUniformLayout::GlobalShaderUniformLayout(
		const std::vector<ShaderDataUniform>& dataUniforms,
		const std::vector<ShaderTextureUniform>& textureUniforms,
		const std::vector<ShaderBufferUniform>& bufferUniforms) :
		ShaderUniformLayout(dataUniforms, textureUniforms),
		BufferUniforms(bufferUniforms)
	{
		CalculateHash();
	}

	bool GlobalShaderUniformLayout::operator==(const GlobalShaderUniformLayout& other) const
	{
		return Hash == other.Hash;
	}

	void GlobalShaderUniformLayout::CalculateHash()
	{
		ShaderUniformLayout::CalculateHash();

		uint64 bufferUniformHash = 0;
		for (const ShaderBufferUniform& u : BufferUniforms)
		{
			bufferUniformHash = Math::CombineHashes(
				bufferUniformHash,
				u.Key,
				static_cast<uint64>(u.BindingPoints),
				static_cast<uint64>(u.Size)
			);
		}

		Hash = Math::CombineHashes(Hash, bufferUniformHash);
	}

	ShaderStageFlags GlobalShaderUniformLayout::GetAllBindStages() const
	{
		return ShaderUniformLayout::GetAllBindStages() | GetBufferUniformBindStages();
	}

	ShaderStageFlags GlobalShaderUniformLayout::GetBufferUniformBindStages() const
	{
		ShaderStageFlags flags = ShaderStageFlags::None;

		for (const ShaderBufferUniform& u : BufferUniforms)
			flags |= u.BindingPoints;

		return flags;
	}
}