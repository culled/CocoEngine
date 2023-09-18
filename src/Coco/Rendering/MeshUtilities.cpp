#include "Renderpch.h"
#include "MeshUtilities.h"

namespace Coco::Rendering
{
	constexpr uint32 CoordsToIndex(uint32 x, uint32 y, uint32 length) { return x * length + y; }

	void MeshUtilities::CreateXYGrid(
		const Vector2& size, 
		const Vector3& offset,
		const VertexDataFormat& format,
		std::vector<VertexData>& vertices, 
		std::vector<uint32>& indices, 
		uint32 subdivisions, 
		bool flipDirection)
	{
		const uint32 vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(size.X, size.Y, 1.0);

		const uint32 indexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Backward : Vector3::Forward;

		for (uint32 x = 0; x < vertexSideCount; x++)
		{
			for (uint32 y = 0; y < vertexSideCount; y++)
			{
				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(y) / (vertexSideCount - 1);

				VertexData& vertex = vertices.emplace_back(Vector3(x * vertexScaling - 0.5, y * vertexScaling - 0.5, 0.0) * scale + offset);

				if (format.HasNormals)
					vertex.Normal = normalDir;

				if (format.HasUV0)
					vertex.UV0 = Vector2(flipDirection ? -u : u, v);
			}
		}

		for (uint32 x = 0; x < vertexSideCount - 1; x++)
		{
			for (uint32 y = 0; y < vertexSideCount - 1; y++)
			{
				const int xy = CoordsToIndex(flipDirection ? x + 1 : x, flipDirection ? y + 1 : y, vertexSideCount);
				const int x1y1 = CoordsToIndex(flipDirection ? x : x + 1, flipDirection ? y : y + 1, vertexSideCount);

				indices.push_back(indexOffset + xy);
				indices.push_back(indexOffset + CoordsToIndex(x, y + 1, vertexSideCount));
				indices.push_back(indexOffset + x1y1);

				indices.push_back(indexOffset + x1y1);
				indices.push_back(indexOffset + CoordsToIndex(x + 1, y, vertexSideCount));
				indices.push_back(indexOffset + xy);
			}
		}
	}
}