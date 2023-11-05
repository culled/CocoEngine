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
		uint64 dataUniformHash = std::accumulate(DataUniforms.begin(), DataUniforms.end(), static_cast<uint64>(0), [](uint64 hash, const ShaderDataUniform& u)
			{
				return Math::CombineHashes(
					hash,
					u.Key,
					static_cast<uint64>(u.BindingPoints),
					static_cast<uint64>(u.Type)
				);
			}
		);

		uint64 textureUniformHash = std::accumulate(TextureUniforms.begin(), TextureUniforms.end(), static_cast<uint64>(0), [](uint64 hash, const ShaderTextureUniform& u)
			{
				return Math::CombineHashes(
					hash,
					u.Key,
					static_cast<uint64>(u.BindingPoints),
					static_cast<uint64>(u.DefaultTexture)
				);
			}
		);

		Hash = Math::CombineHashes(dataUniformHash, textureUniformHash);
	}

	ShaderStageFlags ShaderUniformLayout::GetAllBindStages() const
	{
		return GetDataUniformBindStages() | GetTextureUniformBindStages();
	}

	ShaderStageFlags ShaderUniformLayout::GetDataUniformBindStages() const
	{
		return std::accumulate(DataUniforms.begin(), DataUniforms.end(), ShaderStageFlags::None, [](ShaderStageFlags f, const ShaderDataUniform& u)
			{
				return f | u.BindingPoints;
			}
		);
	}

	ShaderStageFlags ShaderUniformLayout::GetTextureUniformBindStages() const
	{
		return std::accumulate(TextureUniforms.begin(), TextureUniforms.end(), ShaderStageFlags::None, [](ShaderStageFlags f, const ShaderTextureUniform& u)
			{
				return f | u.BindingPoints;
			}
		);
	}

	uint64 ShaderUniformLayout::GetUniformDataSize(const GraphicsDevice& device) const
	{
		uint64 offset = 0;

		for (const auto& u : DataUniforms)
		{
			device.AlignOffset(u.Type, offset);
			offset += GetBufferDataTypeSize(u.Type);
		}

		// Pad out the data size so they fill a block accessible by the minimum buffer alignment
		offset = GraphicsDevice::GetOffsetForAlignment(offset, device.GetFeatures().MinimumBufferAlignment);

		return offset;
	}

	void ShaderUniformLayout::GetBufferFriendlyData(const GraphicsDevice& device, const ShaderUniformData& data, std::vector<uint8>& outBufferData) const
	{
		uint64 offset = 0;

		for (const ShaderDataUniform& uniform : DataUniforms)
		{
			const uint8 dataSize = GetBufferDataTypeSize(uniform.Type);
			device.AlignOffset(uniform.Type, offset);

			outBufferData.resize(offset + dataSize);
			uint8* dst = outBufferData.data() + offset;

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
		outBufferData.resize(GraphicsDevice::GetOffsetForAlignment(outBufferData.size(), device.GetFeatures().MinimumBufferAlignment));
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

		uint64 bufferUniformHash = std::accumulate(BufferUniforms.begin(), BufferUniforms.end(), static_cast<uint64>(0), [](uint64 hash, const ShaderBufferUniform& u)
			{
				return Math::CombineHashes(
					hash,
					u.Key,
					static_cast<uint64>(u.BindingPoints),
					static_cast<uint64>(u.Size)
				);
			}
		);

		Hash = Math::CombineHashes(Hash, bufferUniformHash);
	}

	ShaderStageFlags GlobalShaderUniformLayout::GetAllBindStages() const
	{
		return ShaderUniformLayout::GetAllBindStages() | GetBufferUniformBindStages();
	}

	ShaderStageFlags GlobalShaderUniformLayout::GetBufferUniformBindStages() const
	{
		return std::accumulate(BufferUniforms.begin(), BufferUniforms.end(), ShaderStageFlags::None, [](ShaderStageFlags f, const ShaderBufferUniform& u)
			{
				return f | u.BindingPoints;
			}
		);
	}
}