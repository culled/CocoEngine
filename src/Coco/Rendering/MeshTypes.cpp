#include "Renderpch.h"
#include "MeshTypes.h"

namespace Coco::Rendering
{
	VertexDataFormat::VertexDataFormat() :
		VertexDataFormat(VertexAttrFlags::None)
	{}

	VertexDataFormat::VertexDataFormat(VertexAttrFlags additionalAttributes) :
		AdditionalAttributes(additionalAttributes)
	{}

	uint64 VertexDataFormat::GetDataSize(uint64 vertexCount) const
	{
		uint64 size = GetBufferDataTypeSize(BufferDataType::Float3) + GetAdditionalAttrStride(); // Position + additional data
		return size * vertexCount;
	}

	uint64 VertexDataFormat::GetAdditionalAttrStride() const
	{
		uint64 stride = 0;

		ForEachAdditionalAttr([&stride](VertexAttrFlags flag, BufferDataType type)
			{
				stride += GetBufferDataTypeSize(type);
			});

		return stride;
	}

	void VertexDataFormat::ForEachAdditionalAttr(std::function<void(VertexAttrFlags, BufferDataType)> callback) const
	{
		for (int i = 1; i < static_cast<int>(VertexAttrFlags::All); i = i << 1)
		{
			VertexAttrFlags flag = static_cast<VertexAttrFlags>(i);
			if ((flag & AdditionalAttributes) != flag)
				continue;

			BufferDataType type;

			switch (flag)
			{
			case VertexAttrFlags::UV0:
				type = BufferDataType::Float2;
				break;
			case VertexAttrFlags::Normal:
				type = BufferDataType::Float3;
				break;
			case VertexAttrFlags::Color:
			case VertexAttrFlags::Tangent:
				type = BufferDataType::Float4;
				break;
			default:
				continue;
			}

			callback(flag, type);
		}
	}

	const uint16 VertexData::MaxVertexSize =
		GetBufferDataTypeSize(BufferDataType::Float3) + // Position
		GetBufferDataTypeSize(BufferDataType::Float3) + // Normal
		GetBufferDataTypeSize(BufferDataType::Float4) + // Color
		GetBufferDataTypeSize(BufferDataType::Float4) + // Tangent
		GetBufferDataTypeSize(BufferDataType::Float2); // UV0

	VertexData::VertexData() :
		VertexData(Vector3::Zero)
	{}

	VertexData::VertexData(const Vector3& position) :
		Position(position),
		Normal(Vector3::Zero),
		Color(Vector4::Zero),
		Tangent(Vector4::Zero),
		UV0(Vector2::Zero)
	{}

	Submesh::Submesh() :
		Submesh(0, 0)
	{}

	Submesh::Submesh(uint64 indexOffset, uint64 indexCount) :
		IndexOffset(indexOffset),
		IndexCount(indexCount)
	{}

	std::vector<uint8> GetVertexBufferData(const VertexDataFormat& format, std::span<const VertexData> vertices)
	{
		std::vector<uint8> vertexData(format.GetDataSize(vertices.size()));

		std::vector<float> temp(VertexData::MaxVertexSize);
		uint64 index = 0;

		// Store vertex positions first
		for (size_t i = 0; i < vertices.size(); i++)
		{
			const VertexData& vertex = vertices[i];

			temp[0] = static_cast<float>(vertex.Position.X);
			temp[1] = static_cast<float>(vertex.Position.Y);
			temp[2] = static_cast<float>(vertex.Position.Z);

			uint64 size = sizeof(float) * 3;
			Assert(memcpy_s(vertexData.data() + index, vertexData.size(), temp.data(), size) == 0)
			index += size;
		}

		// Now interleave the additional vertex data
		if (format.AdditionalAttributes != VertexAttrFlags::None)
		{
			for (size_t i = 0; i < vertices.size(); i++)
			{
				uint16 p = 0;
				const VertexData& vertex = vertices[i];

				if ((format.AdditionalAttributes & VertexAttrFlags::Normal) == VertexAttrFlags::Normal)
				{
					temp[p++] = static_cast<float>(vertex.Normal.X);
					temp[p++] = static_cast<float>(vertex.Normal.Y);
					temp[p++] = static_cast<float>(vertex.Normal.Z);
				}

				if ((format.AdditionalAttributes & VertexAttrFlags::Color) == VertexAttrFlags::Color)
				{
					temp[p++] = static_cast<float>(vertex.Color.X);
					temp[p++] = static_cast<float>(vertex.Color.Y);
					temp[p++] = static_cast<float>(vertex.Color.Z);
					temp[p++] = static_cast<float>(vertex.Color.W);
				}

				if ((format.AdditionalAttributes & VertexAttrFlags::Tangent) == VertexAttrFlags::Tangent)
				{
					temp[p++] = static_cast<float>(vertex.Tangent.X);
					temp[p++] = static_cast<float>(vertex.Tangent.Y);
					temp[p++] = static_cast<float>(vertex.Tangent.Z);
					temp[p++] = static_cast<float>(vertex.Tangent.W);
				}

				if ((format.AdditionalAttributes & VertexAttrFlags::UV0) == VertexAttrFlags::UV0)
				{
					temp[p++] = static_cast<float>(vertex.UV0.X);
					temp[p++] = static_cast<float>(vertex.UV0.Y);
				}

				if (p == 0)
					break;

				size_t tempSize = p * sizeof(float);
				Assert(memcpy_s(vertexData.data() + index, vertexData.size(), temp.data(), tempSize) == 0)
					index += tempSize;
			}
		}

		return vertexData;
	}

	BoundingBox CalculateBounds(std::span<const VertexData> vertices)
	{
		std::vector<uint32> indices;
		indices.reserve(vertices.size());

		for (uint32 i = 0; i < vertices.size(); ++i)
			indices.push_back(i);

		return CalculateBounds(vertices, indices);
	}

	BoundingBox CalculateBounds(std::span<const VertexData> vertices, std::span<const uint32> indices)
	{
		BoundingBox bounds;

		bool first = true;
		for (const uint32& i : indices)
		{
			const VertexData& v = vertices[i];

			if (first)
			{
				bounds = BoundingBox(v.Position, v.Position);
				first = false;
			}
			else
			{
				bounds.Expand(v.Position);
			}
		}

		return bounds;
	}
}