#include "MeshPrimitives.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	constexpr uint CoordsToIndex(uint x, uint y, uint length) { return x * length + y; }

	Ref<Mesh> MeshPrimitives::CreateXYPlane(const string& name, const Vector2& size, uint subdivisions)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateXYGrid(size, Vector3::Zero, verts, uvs, indices, subdivisions);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateXZPlane(const string& name, const Vector2& size, uint subdivisions)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateXZGrid(size, Vector3::Zero, verts, uvs, indices, subdivisions);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateYZPlane(const string& name, const Vector2& size, uint subdivisions)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateYZGrid(size, Vector3::Zero, verts, uvs, indices, subdivisions);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	Ref<Mesh> MeshPrimitives::CreateBox(const string& name, const Vector3& size, uint subdivisions)
	{
		List<Vector3> verts;
		List<Vector2> uvs;
		List<uint> indices;

		CreateBox(size, Vector3::Zero, verts, uvs, indices, subdivisions);

		return CreateFromVertices(name, verts, uvs, indices);
	}

	void MeshPrimitives::CreateXYGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = (1.0 / vertexSideCount) * 2.0;
		const Vector3 scale = Vector3(size.X * vertexScaling, size.Y * vertexScaling, 1.0);
		const Vector3 finalOffset = Vector3(-size.X * 0.5, -size.Y * 0.5, 0.0) + offset;

		const uint indexOffset = static_cast<uint>(positions.Count());

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint y = 0; y < vertexSideCount; y++)
			{
				positions.Add(Vector3(x, y, 0.0) * scale + finalOffset);

				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(y) / (vertexSideCount - 1);
				uvs.Construct(u, v);
			}
		}

		for (uint x = 0; x < vertexSideCount - 1; x++)
		{
			for (uint y = 0; y < vertexSideCount - 1; y++)
			{
				indices.Add(indexOffset + CoordsToIndex(x,		y,		vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x,		y + 1,	vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x + 1,	y + 1,	vertexSideCount));

				indices.Add(indexOffset + CoordsToIndex(x + 1,	y + 1,	vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x + 1,	y,		vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x,		y,		vertexSideCount));
			}
		}
	}

	void MeshPrimitives::CreateXZGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = (1.0 / vertexSideCount) * 2.0;
		const Vector3 scale = Vector3(size.X * vertexScaling, 1.0, size.Y * vertexScaling);
		const Vector3 finalOffset = Vector3(-size.X * 0.5, 0.0, -size.Y * 0.5) + offset;

		const uint indexOffset = static_cast<uint>(positions.Count());

		for (uint x = 0; x < vertexSideCount; x++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				positions.Add(Vector3(x, 0.0, z) * scale + finalOffset);

				const double u = static_cast<double>(x) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);
				uvs.Construct(u, v);
			}
		}

		for (uint x = 0; x < vertexSideCount - 1; x++)
		{
			for (uint z = 0; z < vertexSideCount - 1; z++)
			{
				indices.Add(indexOffset + CoordsToIndex(x + 1,	z,		vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x + 1,	z + 1,	vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x,		z + 1,	vertexSideCount));

				indices.Add(indexOffset + CoordsToIndex(x,		z + 1,	vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x,		z,		vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(x + 1,	z,		vertexSideCount));
			}
		}
	}

	void MeshPrimitives::CreateYZGrid(const Vector2& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions)
	{
		const uint vertexSideCount = 2 + subdivisions;
		const double vertexScaling = (1.0 / vertexSideCount) * 2.0;
		const Vector3 scale = Vector3(1.0, size.X * vertexScaling, size.Y * vertexScaling);
		const Vector3 finalOffset = Vector3(0.0, -size.X * 0.5, -size.Y * 0.5) + offset;

		const uint indexOffset = static_cast<uint>(positions.Count());

		for (uint y = 0; y < vertexSideCount; y++)
		{
			for (uint z = 0; z < vertexSideCount; z++)
			{
				positions.Add(Vector3(0.0, y, z) * scale + finalOffset);

				const double u = static_cast<double>(y) / (vertexSideCount - 1);
				const double v = static_cast<double>(z) / (vertexSideCount - 1);
				uvs.Construct(u, v);
			}
		}

		for (uint y = 0; y < vertexSideCount - 1; y++)
		{
			for (uint z = 0; z < vertexSideCount - 1; z++)
			{
				indices.Add(indexOffset + CoordsToIndex(y, z, vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(y, z + 1, vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(y + 1, z + 1, vertexSideCount));

				indices.Add(indexOffset + CoordsToIndex(y + 1, z + 1, vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(y + 1, z, vertexSideCount));
				indices.Add(indexOffset + CoordsToIndex(y, z, vertexSideCount));
			}
		}
	}
	
	void MeshPrimitives::CreateBox(const Vector3& size, const Vector3& offset, List<Vector3>& positions, List<Vector2>& uvs, List<uint>& indices, uint subdivisions)
	{
		const Vector3 sizeOffset = size / 2;

		// -Y face
		CreateXZGrid(Vector2(-size.X, size.Z), Vector3::Backwards * sizeOffset.Y + offset, positions, uvs, indices, subdivisions);

		// X face
		CreateYZGrid(Vector2(size.Y, size.Z), Vector3::Right * sizeOffset.X + offset, positions, uvs, indices, subdivisions);

		// Y face
		CreateXZGrid(Vector2(size.X, size.Z), Vector3::Forwards * sizeOffset.Y + offset, positions, uvs, indices, subdivisions);

		// -X face
		CreateYZGrid(Vector2(-size.Y, size.Z), Vector3::Left * sizeOffset.X + offset, positions, uvs, indices, subdivisions);

		// Z face
		CreateXYGrid(Vector2(size.X, size.Z), Vector3::Up * sizeOffset.Z + offset, positions, uvs, indices, subdivisions);

		// -Z face
		CreateXYGrid(Vector2(-size.X, size.Z), Vector3::Down * sizeOffset.Z + offset, positions, uvs, indices, subdivisions);
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