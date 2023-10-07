#include "Renderpch.h"
#include "Mesh.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	SubMesh::SubMesh() :
		SubMesh(0, 0, BoundingBox::Zero)
	{}

	SubMesh::SubMesh(uint64 offset, uint64 count, const BoundingBox& bounds) :
		Offset(offset),
		Count(count),
		Bounds(bounds)
	{}

	Mesh::Mesh(const ResourceID& id, const string& name, bool isDynamic) :
		RendererResource(id, name),
		_vertexBuffer(),
		_indexBuffer(),
		_vertexFormat(),
		_vertices{},
		_indices{},
		_submeshes{},
		_vertexCount(0),
		_indexCount(0),
		_isDynamic(isDynamic),
		_isDirty(true),
		_lockedVertexMemory(nullptr),
		_lockedIndexMemory(nullptr),
		_meshBounds()
	{}

	Mesh::~Mesh()
	{
		RenderService* rendering = RenderService::Get();
		if (rendering)
		{
			GraphicsDevice& device = rendering->GetDevice();

			device.TryReleaseBuffer(_vertexBuffer);
			device.TryReleaseBuffer(_indexBuffer);
		}
	}

	void Mesh::SetVertices(const VertexDataFormat& format, std::span<const VertexData> vertices)
	{
		_vertices.resize(vertices.size());
		Assert(memcpy_s(_vertices.data(), _vertices.size() * sizeof(VertexData), vertices.data(), vertices.size() * sizeof(VertexData)) == 0)

		_vertexCount = _vertices.size();
		_vertexFormat = format;

		bool first = true;
		for (const VertexData& v : vertices)
		{
			if (first)
			{
				_meshBounds = BoundingBox(v.Position, v.Position);
				first = false;
			}
			else
			{
				_meshBounds.Expand(v.Position);
			}
		}

		MarkDirty();
	}

	void Mesh::SetIndices(std::span<const uint32> indices, uint32 submeshID)
	{
		std::vector<uint32>& submeshIndices = _indices[submeshID];
		submeshIndices.resize(indices.size());
		Assert(memcpy_s(submeshIndices.data(), submeshIndices.size() * sizeof(uint32), indices.data(), indices.size() * sizeof(uint32)) == 0)

		_indexCount = 0;
		for (const auto& kvp : _indices)
		{
			_indexCount += kvp.second.size();
		}

		MarkDirty();
	}

	bool Mesh::Apply(bool deleteLocalData)
	{
		if (!_isDirty)
			return true;

		if (!RenderService::Get())
			throw std::exception("No active RenderService found");

		RenderService& rendering = *RenderService::Get();

		Ref<Buffer> stagingBuffer;
		bool success = false;

		try
		{
			if (_vertexCount == 0)
				throw std::exception("No vertex data has been set");

			if (_indices.size() == 0)
				throw std::exception("No index data has been set");

			for (const auto& kvp : _indices)
				if (kvp.second.size() == 0)
					throw std::exception("Submesh index list was empty");

			// Gather vertex data
			std::vector<uint8> vertexBufferData = GetVertexData(_vertexFormat, _vertices);

			if (!_vertexBuffer.IsValid())
			{
				BufferUsageFlags vertexFlags = BufferUsageFlags::Vertex | BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource;

				if (_isDynamic)
					vertexFlags |= BufferUsageFlags::HostVisible;

				_vertexBuffer = rendering.GetDevice().CreateBuffer(
					vertexBufferData.size(), 
					vertexFlags,
					true);

				_lockedVertexMemory = nullptr;
			}
			else if(_vertexBuffer->GetSize() < vertexBufferData.size())
			{
				_vertexBuffer->Resize(vertexBufferData.size(), false);
				_lockedVertexMemory = nullptr;
			}

			// Gather index data
			uint64 offset = 0;
			std::vector<uint8> indexBufferData(_indexCount * sizeof(uint32));
			for (const auto& kvp : _indices)
			{
				const std::vector<uint32>& submeshIndices = kvp.second;

				Assert(memcpy_s(indexBufferData.data() + offset * sizeof(uint32), indexBufferData.size(), submeshIndices.data(), sizeof(uint32) * submeshIndices.size()) == 0)

				offset += submeshIndices.size();
			}

			if (!_indexBuffer.IsValid())
			{
				BufferUsageFlags indexFlags = BufferUsageFlags::Index | BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource;

				if (_isDynamic)
					indexFlags |= BufferUsageFlags::HostVisible;

				_indexBuffer = rendering.GetDevice().CreateBuffer(
					indexBufferData.size(),
					indexFlags,
					true);

				_lockedIndexMemory = nullptr;
			}
			else if(_indexBuffer->GetSize() < indexBufferData.size())
			{
				_indexBuffer->Resize(indexBufferData.size(), false);
				_lockedIndexMemory = nullptr;
			}

			if (_isDynamic)
			{
				if (!_lockedVertexMemory)
					_lockedVertexMemory = _vertexBuffer->Lock(0, vertexBufferData.size());

				if (!_lockedIndexMemory)
					_lockedIndexMemory = _indexBuffer->Lock(0, indexBufferData.size());

				Assert(memcpy_s(_lockedVertexMemory, _vertexBuffer->GetSize(), vertexBufferData.data(), vertexBufferData.size()) == 0)
				Assert(memcpy_s(_lockedIndexMemory, _indexBuffer->GetSize(), indexBufferData.data(), indexBufferData.size()) == 0)
			}
			else
			{
				stagingBuffer = rendering.GetDevice().CreateBuffer(
					vertexBufferData.size(),
					BufferUsageFlags::HostVisible | BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource,
					true);

				// Upload vertex data
				stagingBuffer->LoadData<uint8>(0, vertexBufferData);
				stagingBuffer->CopyTo(0, *_vertexBuffer, 0, vertexBufferData.size());

				// Upload index data
				stagingBuffer->LoadData<uint8>(0, indexBufferData);
				stagingBuffer->CopyTo(0, *_indexBuffer, 0, indexBufferData.size());
			}

			if (_submeshes.size() != _indices.size())
				CalculateSubmeshes();

			if (deleteLocalData)
			{
				_vertices.clear();
				_indices.clear();
			}

			success = true;
			_isDirty = false;
		}
		catch (const std::exception& ex)
		{
			CocoError("Failed to upload mesh data: {}", ex.what())			
		}

		if (stagingBuffer.IsValid())
		{
			rendering.GetDevice().TryReleaseBuffer(stagingBuffer);
		}

		return success;
	}

	void Mesh::ClearSubmeshes()
	{
		_indexCount = 0;
		_indices.clear();
		_submeshes.clear();

		MarkDirty();
	}

	bool Mesh::TryGetSubmesh(uint32 submeshID, SubMesh& outSubmesh) const
	{
		if (!_submeshes.contains(submeshID))
			return false;

		outSubmesh = _submeshes.at(submeshID);
		return true;
	}

	void Mesh::CalculateSubmeshes()
	{
		_submeshes.clear();

		uint64 offset = 0;
		for (const auto& kvp : _indices)
		{
			const std::vector<uint32>& submeshIndices = kvp.second;

			BoundingBox b = CalculateSubmeshBounds(kvp.first);

			_submeshes.try_emplace(kvp.first, offset, submeshIndices.size(), b);

			offset += submeshIndices.size();
		}
	}

	BoundingBox Mesh::CalculateSubmeshBounds(uint32 submeshID)
	{
		const std::vector<uint32>& submeshIndices = _indices.at(submeshID);

		BoundingBox b;
		bool first = true;

		for (const uint32& vI : submeshIndices)
		{
			const VertexData& vertex = _vertices.at(vI);

			if (first)
			{
				b = BoundingBox(vertex.Position, vertex.Position);
				first = false;
			}
			else
			{
				b.Expand(vertex.Position);
			}
		}

		return b;
	}

	void Mesh::MarkDirty()
	{
		_isDirty = true;
	}
}