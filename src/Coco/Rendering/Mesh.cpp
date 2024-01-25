#include "Renderpch.h"
#include "Mesh.h"

#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	Mesh::Mesh(const ResourceID& id, MeshUsageFlags usageFlags) :
		Resource(id),
		_usageFlags(usageFlags),
		_isDirty(false),
		_vertexFormat(),
		_vertices(),
		_vertexDataSize(0),
		_vertexCount(0),
		_indices(),
		_indexDataSize(0),
		_indexCount(0),
		_submeshes(),
		_localBounds()
	{}

	void Mesh::SetVertices(const VertexDataFormat & format, std::span<const VertexData> vertices)
	{
		_vertexFormat = format;
		_vertices.resize(vertices.size());
		Assert(memcpy_s(_vertices.data(), _vertices.size() * sizeof(VertexData), vertices.data(), vertices.size() * sizeof(VertexData)) == 0)

		_localBounds = CalculateBounds(vertices);
		_vertexCount = vertices.size();
		_vertexDataSize = format.GetDataSize(_vertexCount);

		MarkDirty();
	}

	void Mesh::SetIndices(const std::unordered_map<uint32, std::vector<uint32>>& submeshIndices)
	{
		_indices.clear();
		_submeshes.clear();

		_indexCount = 0;
		for (const auto& kvp : submeshIndices)
		{
			_indices.resize(_indices.size() + kvp.second.size());
			Assert(memcpy_s(_indices.data() + _indexCount, _indices.size() * sizeof(uint32), kvp.second.data(), kvp.second.size() * sizeof(uint32)) == 0)

			_submeshes.try_emplace(kvp.first, _indexCount, kvp.second.size());
			_indexCount += kvp.second.size();
		}

		_indexDataSize = _indexCount * sizeof(uint32);

		MarkDirty();
	}

	void Mesh::SetIndices(std::span<const uint32> indices, const std::unordered_map<uint32, Submesh>& submeshes)
	{
		_indices = std::vector<uint32>(indices.begin(), indices.end());
		_submeshes = submeshes;

		_indexCount = _indices.size();
		_indexDataSize = _indexCount * sizeof(uint32);

		MarkDirty();
	}

	void Mesh::SetIndices(std::span<const uint32> indices)
	{
		SetIndices(indices, { {0, Submesh(0, indices.size())} });
	}

	bool Mesh::TryGetSubmesh(uint32 id, const Submesh*& outSubmesh) const
	{
		auto it = _submeshes.find(id);

		if (it == _submeshes.end())
			return false;

		outSubmesh = &it->second;
		return true;
	}

	void Mesh::GetDataSizeRequirements(uint64& outVertexDataSize, uint64& outIndexDataSize) const
	{
		outVertexDataSize = _vertexDataSize;
		outIndexDataSize = _indexDataSize;
	}

	bool Mesh::UploadData(void* vertexBufferMemory, uint64 vertexMemorySize, void* indexBufferMemory, uint64 indexMemorySize)
	{
		CocoAssert(vertexMemorySize == _vertexDataSize,
			FormatString("Vertex memory size ({}) does not equal vertex data size ({})", vertexMemorySize, _vertexDataSize))
		CocoAssert(indexMemorySize == _indexDataSize,
			FormatString("Index memory size ({}) does not equal index data size ({})", indexMemorySize, _indexDataSize))

		// Copy vertex data
		std::vector<uint8> vertexBufferData = GetVertexBufferData(_vertexFormat, _vertices);
		Assert(memcpy_s(vertexBufferMemory, vertexMemorySize, (char*)vertexBufferData.data(), vertexBufferData.size()) == 0)

		// Copy index data
		Assert(memcpy_s(indexBufferMemory, indexMemorySize, (char*)_indices.data(), _indexDataSize) == 0)

		if ((_usageFlags & MeshUsageFlags::Dynamic) != MeshUsageFlags::Dynamic && (_usageFlags & MeshUsageFlags::RetainCPUData) != MeshUsageFlags::RetainCPUData)
		{
			_vertices.clear();
			_indices.clear();
		}

		_isDirty = false;

		return true;
	}

	void Mesh::MarkDirty()
	{
		if (_isDirty)
			return;

		_isDirty = true;
		IncrementVersion();
	}
}