#include "Renderpch.h"
#include "Mesh.h"

#include "RenderService.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering
{
	SubMesh::SubMesh() :
		SubMesh(0, 0)
	{}

	SubMesh::SubMesh(uint64 offset, uint64 count) :
		Offset(offset),
		Count(count)
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
		_isDirty(true)
	{}

	Mesh::~Mesh()
	{
		_vertexBuffer.Invalidate();
		_indexBuffer.Invalidate();

		RenderService* rendering = RenderService::Get();
		if(rendering)
			rendering->GetDevice()->PurgeUnusedResources();
	}

	void Mesh::SetVertices(const VertexDataFormat& format, std::span<VertexData> vertices)
	{
		_vertices.resize(vertices.size());
		Assert(memcpy_s(_vertices.data(), _vertices.size() * sizeof(VertexData), vertices.data(), vertices.size() * sizeof(VertexData)) == 0)

		_vertexCount = _vertices.size();
		_vertexFormat = format;

		MarkDirty();
	}

	void Mesh::SetIndices(std::span<uint32> indices, uint32 submeshID)
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

		Ref<Buffer> stagingBuffer;
		bool success = false;

		try
		{
			if(!RenderService::Get())
				throw std::exception("No active RenderService found");

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

				_vertexBuffer = RenderService::Get()->GetDevice()->CreateBuffer(
					vertexBufferData.size(), 
					vertexFlags,
					true);
			}
			else
			{
				_vertexBuffer->Resize(vertexBufferData.size(), false);
			}

			_submeshes.clear();

			// Gather index data
			uint64 offset = 0;
			std::vector<uint8> indexBufferData(_indexCount * sizeof(uint32));
			for (const auto& kvp : _indices)
			{
				const std::vector<uint32>& submeshIndices = kvp.second;

				_submeshes.try_emplace(kvp.first, offset, submeshIndices.size());

				Assert(memcpy_s(indexBufferData.data() + offset * sizeof(uint32), indexBufferData.size(), submeshIndices.data(), sizeof(uint32) * submeshIndices.size()) == 0)

				offset += submeshIndices.size();
			}

			if (!_indexBuffer.IsValid())
			{
				BufferUsageFlags indexFlags = BufferUsageFlags::Index | BufferUsageFlags::TransferDestination | BufferUsageFlags::TransferSource;

				if (_isDynamic)
					indexFlags |= BufferUsageFlags::HostVisible;

				_indexBuffer = RenderService::Get()->GetDevice()->CreateBuffer(
					indexBufferData.size(),
					indexFlags,
					true);
			}
			else
			{
				_indexBuffer->Resize(indexBufferData.size(), false);
			}

			if (_isDynamic)
			{
				_vertexBuffer->LoadData<uint8>(0, vertexBufferData);
				_indexBuffer->LoadData<uint8>(0, indexBufferData);
			}
			else
			{
				stagingBuffer = RenderService::Get()->GetDevice()->CreateBuffer(
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
			stagingBuffer.Invalidate();
			RenderService::Get()->GetDevice()->PurgeUnusedResources();
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

	void Mesh::MarkDirty()
	{
		_isDirty = true;
	}
}