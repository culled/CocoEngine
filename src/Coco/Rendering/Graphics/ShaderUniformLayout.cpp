#include "Renderpch.h"
#include "ShaderUniformLayout.h"
#include "GraphicsDevice.h"
#include "ShaderUniformData.h"

namespace Coco::Rendering
{
	const uint64 ShaderUniformLayout::EmptyHash = 0;

	ShaderUniformLayout::ShaderUniformLayout() :
		Uniforms(),
		Hash(EmptyHash)
	{}

	ShaderUniformLayout::ShaderUniformLayout(const std::vector<ShaderUniform>& uniforms) :
		Uniforms(uniforms)
	{
		this->CalculateHash();
	}

	bool ShaderUniformLayout::operator==(const ShaderUniformLayout& other) const
	{
		return Hash == other.Hash;
	}

	ShaderUniformUnion ShaderUniformLayout::GetDefaultDataUniformValue(const ShaderUniform& uniform)
	{
		ShaderUniformUnion u;

		if (!IsDataShaderUniformType(uniform.Type))
			return u;

		BufferDataType type = GetBufferDataType(uniform.Type);
		switch (type)
		{
		case BufferDataType::Float:
		{
			if (const float* v = std::any_cast<float>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Float2:
		{
			if (const Vector2* v = std::any_cast<Vector2>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Float3:
		{
			if (const Vector3* v = std::any_cast<Vector3>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Float4:
		{
			if (const Vector4* v = std::any_cast<Vector4>(&uniform.DefaultValue))
			{
				u = *v;
			}
			else if (const Color* v = std::any_cast<Color>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Mat4x4:
		{
			if (const Matrix4x4* v = std::any_cast<Matrix4x4>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Int:
		{
			if (const int* v = std::any_cast<int>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Int2:
		{
			if (const Vector2Int* v = std::any_cast<Vector2Int>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Int3:
		{
			if (const Vector3Int* v = std::any_cast<Vector3Int>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Int4:
		{
			if (const Vector4Int* v = std::any_cast<Vector4Int>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		case BufferDataType::Bool:
		{
			if (const bool* v = std::any_cast<bool>(&uniform.DefaultValue))
			{
				u = *v;
			}
			break;
		}
		default:
			break;
		}

		return u;
	}

	void ShaderUniformLayout::CalculateHash()
	{
		Hash = std::accumulate(Uniforms.begin(), Uniforms.end(), static_cast<uint64>(0), [](uint64 hash, const ShaderUniform& u)
			{
				return Math::CombineHashes(
					hash,
					u.Key,
					static_cast<uint64>(u.BindingPoints),
					static_cast<uint64>(u.Type)
				);
			}
		);
	}

	ShaderStageFlags ShaderUniformLayout::GetUniformBindStages(bool dataUniforms, bool textureUniforms) const
	{
		return std::accumulate(Uniforms.begin(), Uniforms.end(), ShaderStageFlags::None, [dataUniforms, textureUniforms](ShaderStageFlags f, const ShaderUniform& u)
			{
				bool data = IsDataShaderUniformType(u.Type);

				if ((data && dataUniforms) || (!data && textureUniforms))
				{
					f |= u.BindingPoints;
				}

				return f;
			}
		);
	}

	uint64 ShaderUniformLayout::GetUniformDataSize(const GraphicsDevice& device) const
	{
		uint64 offset = 0;

		for (const auto& u : Uniforms)
		{
			if (!IsDataShaderUniformType(u.Type))
				continue;

			BufferDataType type = GetBufferDataType(u.Type);
			device.AlignOffset(type, offset);
			offset += GetBufferDataTypeSize(type);
		}

		// Pad out the data size so they fill a block accessible by the minimum buffer alignment
		offset = GraphicsDevice::GetOffsetForAlignment(offset, device.GetFeatures().MinimumBufferAlignment);

		return offset;
	}

	void ShaderUniformLayout::GetBufferFriendlyData(const GraphicsDevice& device, const ShaderUniformData& data, std::vector<uint8>& outBufferData) const
	{
		uint64 offset = 0;

		for (const ShaderUniform& uniform : Uniforms)
		{
			if (!IsDataShaderUniformType(uniform.Type))
				continue;

			BufferDataType type = GetBufferDataType(uniform.Type);
			const uint8 dataSize = GetBufferDataTypeSize(type);
			device.AlignOffset(type, offset);

			outBufferData.resize(offset + dataSize);
			uint8* dst = outBufferData.data() + offset;

			ShaderUniformUnion u = data.Uniforms.contains(uniform.Key) ? data.Uniforms.at(uniform.Key) : GetDefaultDataUniformValue(uniform);
			Assert(memcpy_s(dst, dataSize, u.AsData, dataSize) == 0)

			offset += dataSize;
		}

		// Pad the buffer to fit in the minimum buffer alignment
		outBufferData.resize(GraphicsDevice::GetOffsetForAlignment(outBufferData.size(), device.GetFeatures().MinimumBufferAlignment));
	}

	bool ShaderUniformLayout::HasDataUniforms() const
	{
		return std::any_of(Uniforms.begin(), Uniforms.end(), [](const auto& u)
			{
				return IsDataShaderUniformType(u.Type);
			}
		);
	}

	bool ShaderUniformLayout::HasTextureUniforms() const
	{
		return std::any_of(Uniforms.begin(), Uniforms.end(), [](const auto& u)
			{
				return u.Type == ShaderUniformType::Texture;
			}
		);
	}

	GlobalShaderUniformLayout::GlobalShaderUniformLayout() :
		ShaderUniformLayout(),
		BufferUniforms()
	{}

	GlobalShaderUniformLayout::GlobalShaderUniformLayout(
		const std::vector<ShaderUniform>& uniforms,
		const std::vector<ShaderBufferUniform>& bufferUniforms) :
		ShaderUniformLayout(uniforms),
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

	ShaderStageFlags GlobalShaderUniformLayout::GetBufferUniformBindStages() const
	{
		return std::accumulate(BufferUniforms.begin(), BufferUniforms.end(), ShaderStageFlags::None, [](ShaderStageFlags f, const ShaderBufferUniform& u)
			{
				return f | u.BindingPoints;
			}
		);
	}
}