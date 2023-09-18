#include "Renderpch.h"
#include "MeshTypes.h"
#include "Graphics/BufferTypes.h"

namespace Coco::Rendering
{
	VertexDataFormat::VertexDataFormat() :
		HasNormals(false),
		HasUV0(false)
	{}

	uint16 VertexDataFormat::GetVertexSize() const
	{
		uint16 size = GetDataTypeSize(BufferDataType::Float3); // Position

		if (HasNormals)
			size += GetDataTypeSize(BufferDataType::Float3);

		if (HasUV0)
			size += GetDataTypeSize(BufferDataType::Float2);

		return size;
	}

	const uint16 VertexData::MaxVertexSize = 
		GetDataTypeSize(BufferDataType::Float3) + // Position
		GetDataTypeSize(BufferDataType::Float3) + // Normal
		GetDataTypeSize(BufferDataType::Float2); // UV0

	VertexData::VertexData() :
		VertexData(Vector3::Zero)
	{}

	VertexData::VertexData(const Vector3 & position) :
		Position(position),
		Normal(Vector3::Zero),
		UV0(Vector2::Zero)
	{}

	std::vector<uint8> GetVertexData(const VertexDataFormat& format, std::span<VertexData> data)
	{
		uint16 vertexSize = format.GetVertexSize();
		std::vector<uint8> vertexData(vertexSize * data.size());

		std::vector<float> temp(VertexData::MaxVertexSize);
		uint64 index = 0;
		for (size_t i = 0; i < data.size(); i++)
		{
			uint16 p = 0;
			const VertexData& vertex = data[i];

			temp[p++] = static_cast<float>(vertex.Position.X);
			temp[p++] = static_cast<float>(vertex.Position.Y);
			temp[p++] = static_cast<float>(vertex.Position.Z);

			if (format.HasNormals)
			{
				temp[p++] = static_cast<float>(vertex.Normal.X);
				temp[p++] = static_cast<float>(vertex.Normal.Y);
				temp[p++] = static_cast<float>(vertex.Normal.Z);
			}

			if (format.HasUV0)
			{
				temp[p++] = static_cast<float>(vertex.UV0.X);
				temp[p++] = static_cast<float>(vertex.UV0.Y);
			}

			size_t tempSize = p * sizeof(float);
			Assert(memcpy_s(vertexData.data() + index, vertexData.size(), temp.data(), tempSize) == 0)
			index += tempSize;
		}

		return vertexData;
	}
}