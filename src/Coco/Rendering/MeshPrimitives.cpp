#include "MeshPrimitives.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	constexpr uint CoordsToIndex(uint x, uint y, uint length) { return x * length + y; }

	Ref<Mesh> MeshPrimitives::CreateXYPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector3> normals;
		List<Vector2> uvs;
		List<uint> indices;

		CreateXYGrid(size, offset, verts, normals, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, normals, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateXZPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector3> normals;
		List<Vector2> uvs;
		List<uint> indices;

		CreateXZGrid(size, offset, verts, normals, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, normals, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateYZPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector3> normals;
		List<Vector2> uvs;
		List<uint> indices;

		CreateYZGrid(size, offset, verts, normals, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, normals, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateBox(const string& name, const Vector3& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector3> normals;
		List<Vector2> uvs;
		List<uint> indices;

		CreateBox(size, offset, verts, normals, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, normals, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateCone(const string& name, double height, double radius, int baseVertexCount, const Vector3& offset, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector3> normals;
		List<Vector2> uvs;
		List<uint> indices;

		CreateCone(height, radius, baseVertexCount, offset, verts, normals, uvs, indices, flipDirection);

		return CreateFromVertices(name, verts, normals, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateUVSphere(const string& name, int slices, int stacks, double radius, const Vector3& offset, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector3> normals;
		List<Vector2> uvs;
		List<uint> indices;

		CreateUVSphere(slices, stacks, radius, offset, verts, normals, uvs, indices, flipDirection);

		return CreateFromVertices(name, verts, normals, uvs, indices);
	}

	void MeshPrimitives::CreateXYGrid(
		const Vector2& size, 
		const Vector3& offset, 
		List<Vector3>& positions,
		List<Vector3>& normals,
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(size.X, size.Y, 1.0);

		const uint indexOffset = static_cast<uint>(positions.Count());

		Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint y = 0; y < vertexSideCount; y++)
			{
				positions.Add(Vector3(x * vertexScaling - 0.5, y * vertexScaling - 0.5, 0.0) * scale + offset);

				normals.Add(normalDir);

				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(y) / (vertexSideCount - 1);
				uvs.Construct(flipDirection ? -u : u, v);
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
		List<Vector3>& positions,
		List<Vector3>& normals,
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(size.X, 1.0, size.Y);

		const uint indexOffset = static_cast<uint>(positions.Count());

		Vector3 normalDir = flipDirection ? Vector3::Backwards : Vector3::Forwards;

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				positions.Add(Vector3(x * vertexScaling - 0.5, 0.0, z * vertexScaling - 0.5) * scale + offset);

				normals.Add(normalDir);

				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);
				uvs.Construct(flipDirection ? u : -u, v);
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
		List<Vector3>& positions,
		List<Vector3>& normals,
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(1.0, size.X, size.Y);

		const uint indexOffset = static_cast<uint>(positions.Count());

		Vector3 normalDir = flipDirection ? Vector3::Left : Vector3::Right;

		for (uint y = 0; y < vertexSideCount; y++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				positions.Add(Vector3(0.0, y * vertexScaling - 0.5, z * vertexScaling - 0.5) * scale + offset);

				normals.Add(normalDir);

				const double u = static_cast<double>(y) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);
				uvs.Construct(flipDirection ? -u : u, v);
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
		List<Vector3>& positions,
		List<Vector3>& normals,
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const Vector3 sizeOffset = size * 0.5;

		// X face
		CreateYZGrid(Vector2(size.Y, size.Z), Vector3::Right * sizeOffset.X + offset, positions, normals, uvs, indices, subdivisions, flipDirection);

		// -X face
		CreateYZGrid(Vector2(size.Y, size.Z), Vector3::Left * sizeOffset.X + offset, positions, normals, uvs, indices, subdivisions, !flipDirection);

		// Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Forwards * sizeOffset.Y + offset, positions, normals, uvs, indices, subdivisions, flipDirection);

		// -Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Backwards * sizeOffset.Y + offset, positions, normals, uvs, indices, subdivisions, !flipDirection);

		// Z face
		CreateXYGrid(Vector2(size.X, size.Z), Vector3::Up * sizeOffset.Z + offset, positions, normals, uvs, indices, subdivisions, flipDirection);

		// -Z face
		CreateXYGrid(Vector2(size.X, size.Z), Vector3::Down * sizeOffset.Z + offset, positions, normals, uvs, indices, subdivisions, !flipDirection);
	}

	void MeshPrimitives::CreateCone(
		double height, 
		double radius, 
		int baseVertexCount, 
		const Vector3& offset, 
		List<Vector3>& positions,
		List<Vector3>& normals,
		List<Vector2>& uvs, 
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(positions.Count());

		Vector3 topPosition = Vector3(0.0, 0.0, height) + offset;
		Vector3 topNormal = flipDirection ? Vector3::Down : Vector3::Up;

		// Add the top vertex
		positions.Add(topPosition);
		normals.Add(topNormal);
		uvs.Add(Vector2(0.25, 0.25));

		for (int i = 0; i < baseVertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / baseVertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			Vector3 pos = Vector3(c * radius, s * radius, 0.0) + offset;
			positions.Add(pos);

			Vector3 toTop = (topPosition - pos).Normalized();
			Vector3 right = toTop.Cross(Vector3::Up);
			normals.Add(right.Cross(toTop).Normalized());

			uvs.Construct(c * 0.5 + 0.25, s * 0.5 + 0.25);
		}

		for (uint i = vertexOffset + 1; i < static_cast<uint>(positions.Count()) - 1; i++)
		{
			indices.Add(flipDirection ? i + 1 : i);
			indices.Add(vertexOffset);
			indices.Add(flipDirection ? i : i + 1);
		}
		
		// Connect the last base vertex to the first
		indices.Add(flipDirection ? vertexOffset + 1 : static_cast<uint>(positions.Count()) - 1);
		indices.Add(vertexOffset);
		indices.Add(flipDirection ? static_cast<uint>(positions.Count()) - 1 : vertexOffset + 1);

		CreateXYTriangleFan(radius, baseVertexCount, offset, positions, normals, uvs, indices, !flipDirection);
	}

	void MeshPrimitives::CreateXYTriangleFan(
		double radius, 
		int vertexCount, 
		const Vector3& offset, 
		List<Vector3>& positions,
		List<Vector3>& normals,
		List<Vector2>& uvs, 
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(positions.Count());

		Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

		// Add the middle vertex
		positions.Add(offset);
		normals.Add(normalDir);
		uvs.Add(Vector2(0.5, 0.5));

		// Create bottom circle
		for (int i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			positions.Add(Vector3(c * radius, s * radius, 0.0) + offset);
			normals.Add(normalDir);
			uvs.Construct(c * 0.5 + 0.5, s * 0.5 + 0.5);
		}

		for (uint i = vertexOffset + 1; i < static_cast<uint>(positions.Count()) - 1; i++)
		{
			indices.Add(flipDirection ? i + 1 : i);
			indices.Add(vertexOffset);
			indices.Add(flipDirection ? i : i + 1);
		}

		// Connect the last base vertex to the first
		indices.Add(flipDirection ? vertexOffset + 1 : static_cast<uint>(positions.Count()) - 1);
		indices.Add(vertexOffset);
		indices.Add(flipDirection ? static_cast<uint>(positions.Count()) - 1 : vertexOffset + 1);
	}

	void MeshPrimitives::CreateUVSphere(
		int slices, 
		int stacks, 
		double radius, 
		const Vector3& offset, 
		List<Vector3>& positions, 
		List<Vector3>& normals, 
		List<Vector2>& uvs, 
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(positions.Count());
 
		// Based on https://danielsieger.com/blog/2021/03/27/generating-spheres.html
		
		const double uOffset = 1.0 / (slices * 2);

		// Add the top vertices
		for (int j = 0; j < slices; j++)
		{
			double u = (static_cast<double>(j) / slices) + uOffset;

			positions.Add(Vector3(0.0, 0.0, radius) + offset);
			normals.Add(flipDirection ? Vector3::Down : Vector3::Up);
			uvs.Add(Vector2(u, 1.0));
		}

		const double twoPI = 2.0 * Math::PI;

		for (int i = 0; i < stacks - 1; i++)
		{
			double v = 1.0 - (static_cast<double>(i + 1) / stacks);
			double phi = Math::PI * static_cast<double>(i + 1) / stacks;

			for (int j = 0; j <= slices; j++)
			{
				double u = static_cast<double>(j) / slices;
				double theta = twoPI * u;

				Vector3 pos = Vector3(Math::Sin(phi) * Math::Cos(theta), Math::Sin(phi) * Math::Sin(theta), Math::Cos(phi));
				positions.Add(pos * radius + offset);
				normals.Add(pos.Normalized() * (flipDirection ? -1.0 : 1.0));
				uvs.Add(Vector2(u, v));
			}
		}

		// Add the bottom vertices
		for (int j = 0; j < slices; j++)
		{
			double u = (static_cast<double>(j) / slices) + uOffset;

			positions.Add(Vector3(0.0, 0.0, -radius) + offset);
			normals.Add(flipDirection ? Vector3::Up : Vector3::Down);
			uvs.Add(Vector2(u, 0.0));
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
		const List<Vector3>& positions,
		const List<Vector3>& normals, 
		const List<Vector2>& uvs, 
		const List<uint>& indices)
	{
		Ref<Mesh> mesh = Engine::Get()->GetResourceLibrary()->CreateResource<Mesh>(name);
		mesh->SetPositions(positions);
		mesh->SetNormals(normals);
		mesh->SetUVs(uvs);
		mesh->SetIndices(indices);

		return mesh;
	}
}