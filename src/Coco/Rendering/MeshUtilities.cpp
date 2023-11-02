#include "Renderpch.h"
#include "MeshUtilities.h"

namespace Coco::Rendering
{
	constexpr uint32 CoordsToIndex(uint32 x, uint32 y, uint32 length) { return x * length + y; }

	void MeshUtilities::CreateXYGrid(
		const Vector2& size, 
		const Vector3& offset,
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
				vertex.Normal = normalDir;
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
				vertex.Normal = normalDir;
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
				vertex.Normal = normalDir;
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
		std::vector<VertexData>& vertices, 
		std::vector<uint32>& indices, 
		uint32 subdivisions, 
		bool flipDirection)
	{
		Vector3 sizeOffset = size * 0.5;

		// X face
		CreateZYGrid(Vector2(size.Z, size.Y), Vector3::Right * sizeOffset.X + offset, vertices, indices, subdivisions, flipDirection);

		// -X face
		CreateZYGrid(Vector2(size.Z, size.Y), Vector3::Left * sizeOffset.X + offset, vertices, indices, subdivisions, !flipDirection);

		// Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Up * sizeOffset.Y + offset, vertices, indices, subdivisions, flipDirection);

		// -Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Down * sizeOffset.Y + offset, vertices, indices, subdivisions, !flipDirection);

		// Z face
		CreateXYGrid(Vector2(size.X, size.Y), Vector3::Backward * sizeOffset.Z + offset, vertices, indices, subdivisions, flipDirection);

		// -Z face
		CreateXYGrid(Vector2(size.X, size.Y), Vector3::Forward * sizeOffset.Z + offset, vertices, indices, subdivisions, !flipDirection);
	}

	void MeshUtilities::CreateXYTriangleFan(
		double radius, 
		uint32 vertexCount, 
		const Vector3& offset, 
		std::vector<VertexData>& vertices, 
		std::vector<uint32>& indices, 
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Forward : Vector3::Backward;

		{
			// Add the middle vertex
			VertexData& v = vertices.emplace_back(offset);
			v.Normal = normalDir;
			v.UV0 = Vector2(0.5, 0.5);
		}

		// Create bottom circle
		for (uint32 i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData& v = vertices.emplace_back(Vector3(c * radius, s * radius, 0.0) + offset);
			v.Normal = normalDir;
			v.UV0 = Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		uint32 firstVert = vertexOffset + 1;
		uint32 lastVert = static_cast<uint32>(vertices.size()) - 1;

		for (uint32 i = firstVert; i < static_cast<uint32>(vertices.size()) - 1; i++)
		{
			indices.push_back(flipDirection ? i + 1 : i);
			indices.push_back(vertexOffset);
			indices.push_back(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.push_back(flipDirection ? firstVert : lastVert);
		indices.push_back(vertexOffset);
		indices.push_back(flipDirection ? lastVert : firstVert);
	}

	void MeshUtilities::CreateXZTriangleFan(
		double radius,
		uint32 vertexCount,
		const Vector3& offset,
		std::vector<VertexData>& vertices,
		std::vector<uint32>& indices,
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

		{
			// Add the middle vertex
			VertexData& v = vertices.emplace_back(offset);
			v.Normal = normalDir;
			v.UV0 = Vector2(0.5, 0.5);
		}

		// Create bottom circle
		for (uint32 i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData& v = vertices.emplace_back(Vector3(c * radius, 0.0, -s * radius) + offset);
			v.Normal = normalDir;
			v.UV0 = Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		uint32 firstVert = vertexOffset + 1;
		uint32 lastVert = static_cast<uint32>(vertices.size()) - 1;

		for (uint32 i = firstVert; i < lastVert; i++)
		{
			indices.push_back(flipDirection ? i + 1 : i);
			indices.push_back(vertexOffset);
			indices.push_back(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.push_back(flipDirection ? firstVert : lastVert);
		indices.push_back(vertexOffset);
		indices.push_back(flipDirection ? lastVert : firstVert);
	}

	void MeshUtilities::CreateZYTriangleFan(
		double radius,
		uint32 vertexCount,
		const Vector3& offset,
		std::vector<VertexData>& vertices,
		std::vector<uint32>& indices,
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		Vector3 normalDir = flipDirection ? Vector3::Left : Vector3::Right;

		{
			// Add the middle vertex
			VertexData& v = vertices.emplace_back(offset);
			v.Normal = normalDir;
			v.UV0 = Vector2(0.5, 0.5);
		}

		// Create bottom circle
		for (uint32 i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData& v = vertices.emplace_back(Vector3(0.0, s * radius, -c * radius) + offset);
			v.Normal = normalDir;
			v.UV0 = Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		uint32 firstVert = vertexOffset + 1;
		uint32 lastVert = static_cast<uint32>(vertices.size()) - 1;

		for (uint32 i = firstVert; i < lastVert; i++)
		{
			indices.push_back(flipDirection ? i + 1 : i);
			indices.push_back(vertexOffset);
			indices.push_back(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.push_back(flipDirection ? firstVert : lastVert);
		indices.push_back(vertexOffset);
		indices.push_back(flipDirection ? lastVert : firstVert);
	}

	void MeshUtilities::CreateCone(
		double height, 
		double radius, 
		uint32 baseVertexCount, 
		const Vector3& offset, 
		std::vector<VertexData>& vertices, 
		std::vector<uint32>& indices, 
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		Vector3 topPosition = Vector3::Up * height + offset;
		Vector3 topNormal = flipDirection ? Vector3::Down : Vector3::Up;

		{
			// Add the top vertex
			VertexData& v = vertices.emplace_back(topPosition);
			v.Normal = topNormal;
			v.UV0 = Vector2(0.25, 0.25);
		}

		for (uint32 i = 0; i < baseVertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / baseVertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData& v = vertices.emplace_back(Vector3(c * radius, 0.0, -s * radius) + offset);

			Vector3 toTop = (topPosition - v.Position).Normalized();
			Vector3 right = toTop.Cross(Vector3::Up);
			v.Normal = right.Cross(toTop).Normalized();

			v.UV0 = Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		uint32 firstVert = vertexOffset + 1;
		uint32 lastVert = static_cast<uint32>(vertices.size()) - 1;

		for (uint32 i = firstVert; i < lastVert; i++)
		{
			indices.push_back(flipDirection ? i + 1 : i);
			indices.push_back(vertexOffset);
			indices.push_back(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.push_back(flipDirection ? firstVert : lastVert);
		indices.push_back(vertexOffset);
		indices.push_back(flipDirection ? lastVert : firstVert);

		CreateXZTriangleFan(radius, baseVertexCount, offset, vertices, indices, !flipDirection);
	}

	void MeshUtilities::CreateUVSphere(
		uint32 slices, uint32 stacks, 
		double radius, 
		const Vector3& offset,  
		std::vector<VertexData>& vertices, 
		std::vector<uint32>& indices, 
		bool flipDirection)
	{
		const uint32 vertexOffset = static_cast<uint32>(vertices.size());

		// Based on https://danielsieger.com/blog/2021/03/27/generating-spheres.html

		const double uOffset = 1.0 / (slices * 2.0);

		// Add the top vertices
		for (uint32 j = 0; j < slices; j++)
		{
			double u = (static_cast<double>(j) / slices) + uOffset;

			VertexData& v = vertices.emplace_back(Vector3(0.0, radius, 0.0) + offset);
			v.Normal = flipDirection ? Vector3::Down : Vector3::Up;
			v.UV0 = Vector2(u, 1.0);
		}

		const double twoPI = 2.0 * Math::PI;

		for (uint32 i = 0; i < stacks - 1; i++)
		{
			double v = 1.0 - ((static_cast<double>(i) + 1.0) / stacks);
			double phi = Math::PI * (static_cast<double>(i) + 1.0) / stacks;

			for (uint32 j = 0; j <= slices; j++)
			{
				double u = static_cast<double>(j) / slices;
				double theta = twoPI * u;

				Vector3 pos = Vector3(Math::Sin(phi) * Math::Cos(theta), Math::Cos(phi), -Math::Sin(phi) * Math::Sin(theta));
				VertexData& vert = vertices.emplace_back(pos * radius + offset);

				vert.Normal = pos.Normalized() * (flipDirection ? -1.0 : 1.0);
				vert.UV0 = Vector2(u, v);
			}
		}

		// Add the bottom vertices
		for (uint32 j = 0; j < slices; j++)
		{
			double u = (static_cast<double>(j) / slices) + uOffset;

			VertexData& v = vertices.emplace_back(Vector3(0.0, -radius, 0.0) + offset);
			v.Normal = flipDirection ? Vector3::Up : Vector3::Down;
			v.UV0 = Vector2(u, 0.0);
		}

		const uint32 topRingOffset = slices;
		const uint32 bottomRingOffset = topRingOffset + (slices + 1) * (stacks - 2);

		// Add the top and bottom triangles
		for (uint32 i = 0; i < slices; i++)
		{
			uint32 i0 = topRingOffset + i;
			uint32 i1 = topRingOffset + i + 1;

			indices.push_back(vertexOffset + i);
			indices.push_back(vertexOffset + (flipDirection ? i0 : i1));
			indices.push_back(vertexOffset + (flipDirection ? i1 : i0));

			i0 = bottomRingOffset + i;
			i1 = bottomRingOffset + i + 1;

			indices.push_back(vertexOffset + bottomRingOffset + i + slices + 1);
			indices.push_back(vertexOffset + (flipDirection ? i1 : i0));
			indices.push_back(vertexOffset + (flipDirection ? i0 : i1));
		}

		// Add the middle triangles
		for (uint32 j = 0; j < stacks - 2; j++)
		{
			uint32 j0 = topRingOffset + j * (slices + 1);
			uint32 j1 = topRingOffset + (j + 1) * (slices + 1);

			for (uint32 i = 0; i <= slices; i++)
			{
				uint32 i0 = j0 + i;
				uint32 i1 = j0 + i + 1;
				uint32 i2 = j1 + i + 1;
				uint32 i3 = j1 + i;

				indices.push_back(vertexOffset + i0);
				indices.push_back(vertexOffset + (flipDirection ? i3 : i1));
				indices.push_back(vertexOffset + (flipDirection ? i1 : i3));

				indices.push_back(vertexOffset + i2);
				indices.push_back(vertexOffset + (flipDirection ? i1 : i3));
				indices.push_back(vertexOffset + (flipDirection ? i3 : i1));
			}
		}
	}

	void MeshUtilities::CalculateNormals(std::vector<VertexData>& vertices, std::span<const uint32> indices)
	{
		std::unordered_map<uint64, Vector3> normals;

		for (uint64 i = 0; i < indices.size(); i += 3)
		{
			const uint32_t i0 = indices[i];
			const uint32_t i1 = indices[i + 1];
			const uint32_t i2 = indices[i + 2];

			Vector3 edge1 = vertices[i1].Position - vertices[i0].Position;
			Vector3 edge2 = vertices[i2].Position - vertices[i0].Position;

			Vector3 normal = edge1.Cross(edge2).Normalized();

			normals[i0] += normal;
			normals[i1] += normal;
			normals[i2] += normal;
		}

		for (uint64 i = 0; i < vertices.size(); i++)
		{
			vertices.at(i).Normal = normals[i].Normalized();
		}
	}

	void MeshUtilities::CalculateTangents(std::vector<VertexData>& vertices, std::span<const uint32> indices)
	{
		std::unordered_map<uint64, Vector4> tangents;

		// https://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal
		for (uint64 i = 0; i < indices.size(); i += 3)
		{
			const uint32 i0 = indices[i];
			const uint32 i1 = indices[i + 1];
			const uint32 i2 = indices[i + 2];

			Vector3 v1 = vertices[i1].Position - vertices[i0].Position;
			Vector3 v2 = vertices[i2].Position - vertices[i0].Position;

			Vector2 t1 = vertices[i1].UV0 - vertices[i0].UV0;
			Vector2 t2 = vertices[i2].UV0 - vertices[i0].UV0;

			Vector3 normal = (vertices[i0].Normal + vertices[i1].Normal + vertices[i2].Normal).Normalized();

			v1 -= normal * v1.Dot(normal);
			v2 -= normal * v2.Dot(normal);

			double handedness = ((t1.X * t2.Y - t1.Y * t2.X) < 0.0) ? -1.0 : 1.0;
			Vector3 s = ((v1 * t2.Y - v2 * t1.Y) * handedness).Normalized();

			Vector4 tangent4(s, handedness);

			tangents[i0] += tangent4;
			tangents[i1] += tangent4;
			tangents[i2] += tangent4;
		}

		for (uint64 i = 0; i < vertices.size(); i++)
		{
			const Vector4& t = tangents.at(i);
			Vector3 xyz(t.X, t.Y, t.Z);
			xyz.Normalize();

			vertices.at(i).Tangent = Vector4(xyz, t.W > 0.0 ? 1.0 : -1.0);
		}
	}
}