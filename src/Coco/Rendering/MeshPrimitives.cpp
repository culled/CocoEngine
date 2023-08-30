#include "MeshPrimitives.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	constexpr uint CoordsToIndex(uint x, uint y, uint length) { return x * length + y; }

	Ref<Mesh> MeshPrimitives::CreateXYPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<VertexData> verts;
		List<uint> indices;

		CreateXYGrid(size, offset, verts, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, indices, false, true);
	}

	Ref<Mesh> MeshPrimitives::CreateXZPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<VertexData> verts;
		List<uint> indices;

		CreateXZGrid(size, offset, verts, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, indices, false, true);
	}

	Ref<Mesh> MeshPrimitives::CreateYZPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<VertexData> verts;
		List<uint> indices;

		CreateYZGrid(size, offset, verts, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, indices, false, true);
	}

	Ref<Mesh> MeshPrimitives::CreateBox(const string& name, const Vector3& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<VertexData> verts;
		List<uint> indices;

		CreateBox(size, offset, verts, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, indices, false, true);
	}

	Ref<Mesh> MeshPrimitives::CreateCone(const string& name, double height, double radius, int baseVertexCount, const Vector3& offset, bool flipDirection)
	{
		List<VertexData> verts;
		List<uint> indices;

		CreateCone(height, radius, baseVertexCount, offset, verts, indices, flipDirection);

		return CreateFromVertices(name, verts, indices, false, true);
	}

	Ref<Mesh> MeshPrimitives::CreateUVSphere(const string& name, uint slices, uint stacks, double radius, const Vector3& offset, bool flipDirection)
	{
		List<VertexData> verts;
		List<uint> indices;

		CreateUVSphere(slices, stacks, radius, offset, verts, indices, flipDirection);

		return CreateFromVertices(name, verts, indices, false, true);
	}

	void MeshPrimitives::CreateXYGrid(
		const Vector2& size, 
		const Vector3& offset,
		List<VertexData>& vertices,
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(size.X, size.Y, 1.0);

		const uint indexOffset = static_cast<uint>(vertices.Count());

		Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint y = 0; y < vertexSideCount; y++)
			{
				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(y) / (vertexSideCount - 1);

				vertices.Add(
					VertexData(
						Vector3(x * vertexScaling - 0.5, y * vertexScaling - 0.5, 0.0) * scale + offset,
						normalDir,
						Vector2(flipDirection ? -u : u, v)
					)
				);
			}
		}

		for (uint x = 0; x < vertexSideCount - 1; x++)
		{
			for (uint y = 0; y < vertexSideCount - 1; y++)
			{
				const int xy = CoordsToIndex(flipDirection ? x + 1 : x, flipDirection ? y + 1 : y, vertexSideCount);
				const int x1y1 = CoordsToIndex(flipDirection ? x : x + 1, flipDirection ? y : y + 1, vertexSideCount);

				indices.Add(indexOffset + xy);
				indices.Add(indexOffset + CoordsToIndex(x, y + 1, vertexSideCount));
				indices.Add(indexOffset + x1y1);

				indices.Add(indexOffset + x1y1);
				indices.Add(indexOffset + CoordsToIndex(x + 1, y, vertexSideCount));
				indices.Add(indexOffset + xy);
			}
		}
	}

	void MeshPrimitives::CreateXZGrid(
		const Vector2& size, 
		const Vector3& offset,
		List<VertexData>& vertices,
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(size.X, 1.0, size.Y);

		const uint indexOffset = static_cast<uint>(vertices.Count());

		Vector3 normalDir = flipDirection ? Vector3::Backwards : Vector3::Forwards;

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);

				vertices.Add(
					VertexData(
						Vector3(x * vertexScaling - 0.5, 0.0, z * vertexScaling - 0.5) * scale + offset,
						normalDir,
						Vector2(flipDirection ? u : -u, v)
					)
				);
			}
		}

		for (uint x = 0; x < vertexSideCount - 1; x++)
		{
			for (uint z = 0; z < vertexSideCount - 1; z++)
			{
				const int xz = CoordsToIndex(flipDirection ? x + 1 : x, flipDirection ? z + 1 : z, vertexSideCount);
				const int x1z1 = CoordsToIndex(flipDirection ? x : x + 1, flipDirection ? z : z + 1, vertexSideCount);

				indices.Add(indexOffset + xz);
				indices.Add(indexOffset + CoordsToIndex(x + 1, z, vertexSideCount));
				indices.Add(indexOffset + x1z1);

				indices.Add(indexOffset + x1z1);
				indices.Add(indexOffset + CoordsToIndex(x, z + 1, vertexSideCount));
				indices.Add(indexOffset + xz);
			}
		}
	}

	void MeshPrimitives::CreateYZGrid(
		const Vector2& size, 
		const Vector3& offset,
		List<VertexData>& vertices,
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(1.0, size.X, size.Y);

		const uint indexOffset = static_cast<uint>(vertices.Count());

		Vector3 normalDir = flipDirection ? Vector3::Left : Vector3::Right;

		for (uint y = 0; y < vertexSideCount; y++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				const double u = static_cast<double>(y) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);

				vertices.Add(
					VertexData(
						Vector3(0.0, y * vertexScaling - 0.5, z * vertexScaling - 0.5) * scale + offset,
						normalDir,
						Vector2(flipDirection ? -u : u, v)
					)
				);
			}
		}

		for (uint y = 0; y < vertexSideCount - 1; y++)
		{
			for (uint z = 0; z < vertexSideCount - 1; z++)
			{
				const int yz = CoordsToIndex(flipDirection ? y + 1 : y, flipDirection ? z + 1 : z, vertexSideCount);
				const int y1z1 = CoordsToIndex(flipDirection ? y : y + 1, flipDirection ? z : z + 1, vertexSideCount);

				indices.Add(indexOffset + yz);
				indices.Add(indexOffset + CoordsToIndex(y, z + 1, vertexSideCount));
				indices.Add(indexOffset + y1z1);

				indices.Add(indexOffset + y1z1);
				indices.Add(indexOffset + CoordsToIndex(y + 1, z, vertexSideCount));
				indices.Add(indexOffset + yz);
			}
		}
	}
	
	void MeshPrimitives::CreateBox(
		const Vector3& size, 
		const Vector3& offset,
		List<VertexData>& vertices,
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const Vector3 sizeOffset = size * 0.5;

		// X face
		CreateYZGrid(Vector2(size.Y, size.Z), Vector3::Right * sizeOffset.X + offset, vertices, indices, subdivisions, flipDirection);

		// -X face
		CreateYZGrid(Vector2(size.Y, size.Z), Vector3::Left * sizeOffset.X + offset, vertices, indices, subdivisions, !flipDirection);

		// Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Forwards * sizeOffset.Y + offset, vertices, indices, subdivisions, flipDirection);

		// -Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Backwards * sizeOffset.Y + offset, vertices, indices, subdivisions, !flipDirection);

		// Z face
		CreateXYGrid(Vector2(size.X, size.Z), Vector3::Up * sizeOffset.Z + offset, vertices, indices, subdivisions, flipDirection);

		// -Z face
		CreateXYGrid(Vector2(size.X, size.Z), Vector3::Down * sizeOffset.Z + offset, vertices, indices, subdivisions, !flipDirection);
	}

	void MeshPrimitives::CreateCone(
		double height, 
		double radius, 
		int baseVertexCount, 
		const Vector3& offset,
		List<VertexData>& vertices,
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(vertices.Count());

		Vector3 topPosition = Vector3(0.0, 0.0, height) + offset;
		Vector3 topNormal = flipDirection ? Vector3::Down : Vector3::Up;

		// Add the top vertex
		vertices.Add(VertexData(topPosition, topNormal, Vector2(0.25, 0.25)));

		for (int i = 0; i < baseVertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / baseVertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			VertexData data{};
			Vector3 pos = Vector3(c * radius, s * radius, 0.0) + offset;

			Vector3 toTop = (topPosition - pos).Normalized();
			Vector3 right = toTop.Cross(Vector3::Up);
			Vector3 normal = right.Cross(toTop).Normalized();

			vertices.Add(VertexData(pos, normal, Vector2(c * 0.5 + 0.25, s * 0.5 + 0.25)));
		}

		for (uint i = vertexOffset + 1; i < static_cast<uint>(vertices.Count()) - 1; i++)
		{
			indices.Add(flipDirection ? i + 1 : i);
			indices.Add(vertexOffset);
			indices.Add(flipDirection ? i : i + 1);
		}
		
		// Connect the last base vertex to the first
		indices.Add(flipDirection ? vertexOffset + 1 : static_cast<uint>(vertices.Count()) - 1);
		indices.Add(vertexOffset);
		indices.Add(flipDirection ? static_cast<uint>(vertices.Count()) - 1 : vertexOffset + 1);

		CreateXYTriangleFan(radius, baseVertexCount, offset, vertices, indices, !flipDirection);
	}

	void MeshPrimitives::CreateXYTriangleFan(
		double radius, 
		int vertexCount, 
		const Vector3& offset,
		List<VertexData>& vertices,
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(vertices.Count());

		Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

		// Add the middle vertex
		vertices.Add(VertexData(offset, normalDir, Vector2(0.5, 0.5)));

		// Create bottom circle
		for (int i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			vertices.Add(
				VertexData(
					Vector3(c * radius, s * radius, 0.0) + offset,
					normalDir,
					Vector2(c * 0.5 + 0.5, s * 0.5 + 0.5)
				)
			);
		}

		for (uint i = vertexOffset + 1; i < static_cast<uint>(vertices.Count()) - 1; i++)
		{
			indices.Add(flipDirection ? i + 1 : i);
			indices.Add(vertexOffset);
			indices.Add(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.Add(flipDirection ? vertexOffset + 1 : static_cast<uint>(vertices.Count()) - 1);
		indices.Add(vertexOffset);
		indices.Add(flipDirection ? static_cast<uint>(vertices.Count()) - 1 : vertexOffset + 1);
	}

	void MeshPrimitives::CreateUVSphere(
		uint slices,
		uint stacks, 
		double radius, 
		const Vector3& offset,
		List<VertexData>& vertices,
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(vertices.Count());
 
		// Based on https://danielsieger.com/blog/2021/03/27/generating-spheres.html
		
		const double uOffset = 1.0 / (slices * 2);

		// Add the top vertices
		for (uint j = 0; j < slices; j++)
		{
			double u = (static_cast<double>(j) / slices) + uOffset;

			vertices.Add(VertexData(Vector3(0.0, 0.0, radius) + offset, flipDirection ? Vector3::Down : Vector3::Up, Vector2(u, 1.0)));
		}

		const double twoPI = 2.0 * Math::PI;

		for (uint i = 0; i < stacks - 1; i++)
		{
			double v = 1.0 - (static_cast<double>(i + 1) / stacks);
			double phi = Math::PI * static_cast<double>(i + 1) / stacks;

			for (uint j = 0; j <= slices; j++)
			{
				double u = static_cast<double>(j) / slices;
				double theta = twoPI * u;

				Vector3 pos = Vector3(Math::Sin(phi) * Math::Cos(theta), Math::Sin(phi) * Math::Sin(theta), Math::Cos(phi));
				vertices.Add(VertexData(pos * radius + offset, pos.Normalized() * (flipDirection ? -1.0 : 1.0), Vector2(u, v)));
			}
		}

		// Add the bottom vertices
		for (uint j = 0; j < slices; j++)
		{
			double u = (static_cast<double>(j) / slices) + uOffset;

			vertices.Add(VertexData(Vector3(0.0, 0.0, -radius) + offset, flipDirection ? Vector3::Up : Vector3::Down, Vector2(u, 0.0)));
		}

		const uint topRingOffset = slices;
		const uint bottomRingOffset = topRingOffset + (slices + 1) * (stacks - 2);

		// Add the top and bottom triangles
		for (uint i = 0; i < slices; i++)
		{
			uint i0 = topRingOffset + i;
			uint i1 = topRingOffset + i + 1;

			indices.Add(vertexOffset + i);
			indices.Add(vertexOffset + (flipDirection ? i0 : i1));
			indices.Add(vertexOffset + (flipDirection ? i1 : i0));

			i0 = bottomRingOffset + i;
			i1 = bottomRingOffset + i + 1;

			indices.Add(vertexOffset + bottomRingOffset + i + slices + 1);
			indices.Add(vertexOffset + (flipDirection ? i1 : i0));
			indices.Add(vertexOffset + (flipDirection ? i0 : i1));
		}

		// Add the middle triangles
		for (uint j = 0; j < stacks - 2; j++)
		{
			uint j0 = topRingOffset + j * (slices + 1);
			uint j1 = topRingOffset + (j + 1) * (slices + 1);

			for (uint i = 0; i <= slices; i++)
			{
				uint i0 = j0 + i;
				uint i1 = j0 + i + 1;
				uint i2 = j1 + i + 1;
				uint i3 = j1 + i;

				indices.Add(vertexOffset + i0);
				indices.Add(vertexOffset + (flipDirection ? i3 : i1));
				indices.Add(vertexOffset + (flipDirection ? i1 : i3));

				indices.Add(vertexOffset + i2);
				indices.Add(vertexOffset + (flipDirection ? i1 : i3));
				indices.Add(vertexOffset + (flipDirection ? i3 : i1));
			}
		}
	}

	Ref<Mesh> MeshPrimitives::CreateFromVertices(
		const string& name,
		const List<VertexData>& vertices,
		const List<uint>& indices,
		bool calculateNormals,
		bool calculateTangents)
	{
		Ref<Mesh> mesh = Engine::Get()->GetResourceLibrary()->CreateResource<Mesh>(name);
		mesh->SetVertexData(vertices);
		mesh->SetIndices(indices, 0);

		if (calculateNormals)
			mesh->CalculateNormals();

		if (calculateTangents)
			mesh->CalculateTangents();

		return mesh;
	}
}