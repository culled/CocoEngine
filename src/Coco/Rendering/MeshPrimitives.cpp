#include "MeshPrimitives.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	constexpr uint CoordsToIndex(uint x, uint y, uint length) { return x * length + y; }

	Ref<Mesh> MeshPrimitives::CreateXYPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateXYGrid(size, offset, verts, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateXZPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateXZGrid(size, offset, verts, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateYZPlane(const string& name, const Vector2& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateYZGrid(size, offset, verts, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateBox(const string& name, const Vector3& size, const Vector3& offset, uint subdivisions, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateBox(size, offset, verts, uvs, indices, subdivisions, flipDirection);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateCone(const string& name, double height, double radius, int baseVertexCount, const Vector3& offset, bool flipDirection)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateCone(height, radius, baseVertexCount, offset, verts, uvs, indices, flipDirection);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	void MeshPrimitives::CreateXYGrid(
		const Vector2& size, 
		const Vector3& offset, 
		List<Vector3>& positions, 
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(size.X, size.Y, 1.0);

		const uint indexOffset = static_cast<uint>(positions.Count());

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint y = 0; y < vertexSideCount; y++)
			{
				positions.Add(Vector3(x * vertexScaling - 0.5, y * vertexScaling - 0.5, 0.0) * scale + offset);

				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(y) / (vertexSideCount - 1);
				uvs.Construct(u, v);
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
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(size.X, 1.0, size.Y);

		const uint indexOffset = static_cast<uint>(positions.Count());

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				positions.Add(Vector3(x * vertexScaling - 0.5, 0.0, z * vertexScaling - 0.5) * scale + offset);

				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);
				uvs.Construct(u, v);
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
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = 1.0 / (vertexSideCount - 1);
		const Vector3 scale = Vector3(1.0, size.X, size.Y);

		const uint indexOffset = static_cast<uint>(positions.Count());

		for (uint y = 0; y < vertexSideCount; y++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				positions.Add(Vector3(0.0, y * vertexScaling - 0.5, z * vertexScaling - 0.5) * scale + offset);

				const double u = static_cast<double>(y) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);
				uvs.Construct(u, v);
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
		List<Vector2>& uvs, 
		List<uint>& indices, 
		uint subdivisions,
		bool flipDirection)
	{
		const Vector3 sizeOffset = size * 0.5;

		// X face
		CreateYZGrid(Vector2(size.Y, size.Z), Vector3::Right * sizeOffset.X + offset, positions, uvs, indices, subdivisions, flipDirection);

		// -X face
		CreateYZGrid(Vector2(size.Y, size.Z), Vector3::Left * sizeOffset.X + offset, positions, uvs, indices, subdivisions, !flipDirection);

		// Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Forwards * sizeOffset.Y + offset, positions, uvs, indices, subdivisions, flipDirection);

		// -Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Backwards * sizeOffset.Y + offset, positions, uvs, indices, subdivisions, !flipDirection);

		// Z face
		CreateXYGrid(Vector2(size.X, size.Z), Vector3::Up * sizeOffset.Z + offset, positions, uvs, indices, subdivisions, flipDirection);

		// -Z face
		CreateXYGrid(Vector2(size.X, size.Z), Vector3::Down * sizeOffset.Z + offset, positions, uvs, indices, subdivisions, !flipDirection);
	}

	void MeshPrimitives::CreateCone(
		double height, 
		double radius, 
		int baseVertexCount, 
		const Vector3& offset, 
		List<Vector3>& positions, 
		List<Vector2>& uvs, 
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(positions.Count());

		// Add the top vertex
		positions.Add(Vector3(0.0, 0.0, height) + offset);
		uvs.Add(Vector2(0.25, 0.25));

		for (int i = 0; i < baseVertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / baseVertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			positions.Add(Vector3(c * radius, s * radius, 0.0) + offset);
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

		CreateXYTriangleFan(radius, baseVertexCount, offset, positions, uvs, indices, !flipDirection);
	}

	void MeshPrimitives::CreateXYTriangleFan(
		double radius, 
		int vertexCount, 
		const Vector3& offset, 
		List<Vector3>& positions, 
		List<Vector2>& uvs, 
		List<uint>& indices, 
		bool flipDirection)
	{
		const uint vertexOffset = static_cast<uint>(positions.Count());

		// Add the middle vertex
		positions.Add(offset);
		uvs.Add(Vector2(0.5, 0.5));

		// Create bottom circle
		for (int i = 0; i < vertexCount; i++)
		{
			const double angle = (static_cast<double>(i) / vertexCount) * Math::PI * 2.0;
			const double c = Math::Cos(angle);
			const double s = Math::Sin(angle);

			positions.Add(Vector3(c * radius, s * radius, 0.0) + offset);
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

	Ref<Mesh> MeshPrimitives::CreateFromVertices(const string& name, const List<Vector3>& positions, const List<Vector2>& uvs, const List<uint>& indices)
	{
		Ref<Mesh> mesh = Engine::Get()->GetResourceLibrary()->CreateResource<Mesh>(name);
		mesh->SetPositions(positions);
		mesh->SetUVs(uvs);
		mesh->SetIndices(indices);

		return mesh;
	}
}