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

		Vector3 normalDir = flipDirection ? Vector3::Forward : Vector3::Backward;

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

	void MeshUtilities::CreateXZGrid(
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
		const Vector3 scale = Vector3(size.X, 1.0, size.Y);

		const uint32 indexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

		for (uint32 x = 0; x < vertexSideCount; x++)
		{
			for (uint32 z = 0; z < vertexSideCount; z++)
			{
				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = 1.0 - (static_cast<double>(z) / (vertexSideCount - 1));

				VertexData& vertex = vertices.emplace_back(Vector3(x * vertexScaling - 0.5, 0.0, z * vertexScaling - 0.5) * scale + offset);

				if (format.HasNormals)
					vertex.Normal = normalDir;

				if (format.HasUV0)
					vertex.UV0 = Vector2(flipDirection ? -u : u, v);
			}
		}

		for (uint32 x = 0; x < vertexSideCount - 1; x++)
		{
			for (uint32 z = 0; z < vertexSideCount - 1; z++)
			{
				const int xz = CoordsToIndex(flipDirection ? x + 1 : x, flipDirection ? z + 1 : z, vertexSideCount);
				const int x1z1 = CoordsToIndex(flipDirection ? x : x + 1, flipDirection ? z : z + 1, vertexSideCount);

				indices.push_back(indexOffset + xz);
				indices.push_back(indexOffset + CoordsToIndex(x + 1, z, vertexSideCount));
				indices.push_back(indexOffset + x1z1);

				indices.push_back(indexOffset + x1z1);
				indices.push_back(indexOffset + CoordsToIndex(x, z + 1, vertexSideCount));
				indices.push_back(indexOffset + xz);
			}
		}
	}

	void MeshUtilities::CreateZYGrid(
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
		const Vector3 scale = Vector3(1.0, size.Y, size.X);

		const uint32 indexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Left : Vector3::Right;

		for (uint32 z = 0; z < vertexSideCount; z++)
		{
			for (uint32 y = 0; y < vertexSideCount; y++)
			{
				const double u = 1.0 - (static_cast<double>(z) / (vertexSideCount - 1));
				const double v = static_cast<double>(y) / (vertexSideCount - 1);

				VertexData& vertex = vertices.emplace_back(Vector3(0.0, y * vertexScaling - 0.5, z * vertexScaling - 0.5) * scale + offset);

				if (format.HasNormals)
					vertex.Normal = normalDir;

				if (format.HasUV0)
					vertex.UV0 = Vector2(flipDirection ? -u : u, v);
			}
		}

		for (uint32 z = 0; z < vertexSideCount - 1; z++)
		{
			for (uint32 y = 0; y < vertexSideCount - 1; y++)
			{
				const int zy = CoordsToIndex(flipDirection ? z + 1 : z, flipDirection ? y + 1 : y, vertexSideCount);
				const int z1y1 = CoordsToIndex(flipDirection ? z : z + 1, flipDirection ? y : y + 1, vertexSideCount);

				indices.push_back(indexOffset + zy);
				indices.push_back(indexOffset + CoordsToIndex(z + 1, y, vertexSideCount));
				indices.push_back(indexOffset + z1y1);

				indices.push_back(indexOffset + z1y1);
				indices.push_back(indexOffset + CoordsToIndex(z, y + 1, vertexSideCount));
				indices.push_back(indexOffset + zy);
			}
		}
	}
	void MeshUtilities::CreateBox(
		const Vector3& size, 
		const Vector3& offset,
		const VertexDataFormat& format,
		std::vector<VertexData>& vertices, 
		std::vector<uint32>& indices, 
		uint32 subdivisions, 
		bool flipDirection)
	{
		Vector3 sizeOffset = size * 0.5;

		// X face
		CreateZYGrid(Vector2(size.Z, size.Y), Vector3::Right * sizeOffset.X + offset, format, vertices, indices, subdivisions, flipDirection);

		// -X face
		CreateZYGrid(Vector2(size.Z, size.Y), Vector3::Left * sizeOffset.X + offset, format, vertices, indices, subdivisions, !flipDirection);

		// Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Up * sizeOffset.Y + offset, format, vertices, indices, subdivisions, flipDirection);

		// -Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Down * sizeOffset.Y + offset, format, vertices, indices, subdivisions, !flipDirection);

		// Z face
		CreateXYGrid(Vector2(size.X, size.Y), Vector3::Backward * sizeOffset.Z + offset, format, vertices, indices, subdivisions, flipDirection);

		// -Z face
		CreateXYGrid(Vector2(size.X, size.Y), Vector3::Forward * sizeOffset.Z + offset, format, vertices, indices, subdivisions, !flipDirection);
	}

	void MeshUtilities::CreateXYTriangleFan(
		double radius, 
		uint32 vertexCount, 
		const Vector3& offset, 
		const VertexDataFormat& format, 
		std::vector<VertexData>& vertices, 
		std::vector<uint32>& indices, 
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Forward : Vector3::Backward;

		// Add the middle vertex
		VertexData& v = vertices.emplace_back(offset);

		if (format.HasNormals)
			v.Normal = normalDir;

		if (format.HasUV0)
			v.UV0 = Vector2(0.5, 0.5);

		// Create bottom circle
		for (uint32 i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData& v = vertices.emplace_back(Vector3(c * radius, s * radius, 0.0) + offset);

			if (format.HasNormals)
				v.Normal = normalDir;

			if (format.HasUV0)
				v.UV0 = Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		for (uint32 i = vertexOffset + 1; i < static_cast<uint32>(vertices.size()) - 1; i++)
		{
			indices.push_back(flipDirection ? i + 1 : i);
			indices.push_back(vertexOffset);
			indices.push_back(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.push_back(flipDirection ? vertexOffset + 1 : static_cast<uint32>(vertices.size()) - 1);
		indices.push_back(vertexOffset);
		indices.push_back(flipDirection ? static_cast<uint32>(vertices.size()) - 1 : vertexOffset + 1);
	}

	void MeshUtilities::CreateXZTriangleFan(
		double radius,
		uint32 vertexCount,
		const Vector3& offset,
		const VertexDataFormat& format,
		std::vector<VertexData>& vertices,
		std::vector<uint32>& indices,
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

		// Add the middle vertex
		VertexData& v = vertices.emplace_back(offset);

		if (format.HasNormals)
			v.Normal = normalDir;

		if (format.HasUV0)
			v.UV0 = Vector2(0.5, 0.5);

		// Create bottom circle
		for (uint32 i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData& v = vertices.emplace_back(Vector3(c * radius, 0.0, -s * radius) + offset);

			if (format.HasNormals)
				v.Normal = normalDir;

			if (format.HasUV0)
				v.UV0 = Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		for (uint32 i = vertexOffset + 1; i < static_cast<uint32>(vertices.size()) - 1; i++)
		{
			indices.push_back(flipDirection ? i + 1 : i);
			indices.push_back(vertexOffset);
			indices.push_back(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.push_back(flipDirection ? vertexOffset + 1 : static_cast<uint32>(vertices.size()) - 1);
		indices.push_back(vertexOffset);
		indices.push_back(flipDirection ? static_cast<uint32>(vertices.size()) - 1 : vertexOffset + 1);
	}

	void MeshUtilities::CreateZYTriangleFan(
		double radius,
		uint32 vertexCount,
		const Vector3& offset,
		const VertexDataFormat& format,
		std::vector<VertexData>& vertices,
		std::vector<uint32>& indices,
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Left : Vector3::Right;

		// Add the middle vertex
		VertexData& v = vertices.emplace_back(offset);

		if (format.HasNormals)
			v.Normal = normalDir;

		if (format.HasUV0)
			v.UV0 = Vector2(0.5, 0.5);

		// Create bottom circle
		for (uint32 i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData& v = vertices.emplace_back(Vector3(0.0, s * radius, -c * radius) + offset);

			if (format.HasNormals)
				v.Normal = normalDir;

			if (format.HasUV0)
				v.UV0 = Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		for (uint32 i = vertexOffset + 1; i < static_cast<uint32>(vertices.size()) - 1; i++)
		{
			indices.push_back(flipDirection ? i + 1 : i);
			indices.push_back(vertexOffset);
			indices.push_back(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.push_back(flipDirection ? vertexOffset + 1 : static_cast<uint32>(vertices.size()) - 1);
		indices.push_back(vertexOffset);
		indices.push_back(flipDirection ? static_cast<uint32>(vertices.size()) - 1 : vertexOffset + 1);
	}
}