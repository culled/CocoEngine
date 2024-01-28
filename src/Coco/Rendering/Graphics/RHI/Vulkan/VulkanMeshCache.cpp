#include "Renderpch.h"
#include "VulkanMeshCache.h"
#include "VulkanGraphicsDevice.h"

#include "../../../Mesh.h"
#include <Coco/Core/Engine.h>

namespace Coco::Rendering::Vulkan
{
	CachedVulkanMesh::CachedVulkanMesh(const SharedRef<Mesh>& mesh) :
		MeshRef(mesh),
		MeshID(mesh->GetID()),
		MeshVersion(mesh->GetVersion()),
		Buffer(),
		AllocBlock(),
		VertexFormat(mesh->GetVertexFormat()),
		VertexCount(mesh->GetVertexCount()),
		IndexBufferOffset(0),
		IndexCount(mesh->GetIndexCount()),
		LastUsageTime(0.0)
	{}

	CachedVulkanMesh::CachedVulkanMesh(const CachedVulkanMesh & otherMesh) :
		MeshRef(otherMesh.MeshRef),
		MeshID(otherMesh.MeshID),
		MeshVersion(otherMesh.MeshVersion),
		Buffer(),
		AllocBlock(),
		VertexFormat(otherMesh.VertexFormat),
		VertexCount(otherMesh.VertexCount),
		IndexBufferOffset(otherMesh.IndexBufferOffset),
		IndexCount(otherMesh.IndexCount),
		LastUsageTime(0.0)
	{}

	void CachedVulkanMesh::Update(const Mesh& mesh)
	{
		MeshVersion = mesh.GetVersion();
		VertexFormat = mesh.GetVertexFormat();
		VertexCount = mesh.GetVertexCount();
		IndexCount = mesh.GetIndexCount();
	}

	void CachedVulkanMesh::Update(const CachedVulkanMesh& otherMesh)
	{
		MeshVersion = otherMesh.MeshVersion;
		VertexFormat = otherMesh.VertexFormat;
		VertexCount = otherMesh.VertexCount;
		IndexBufferOffset = otherMesh.IndexBufferOffset;
		IndexCount = otherMesh.IndexCount;
	}

	void CachedVulkanMesh::Use()
	{
		LastUsageTime = MainLoop::Get()->GetCurrentTick().Time;
	}

	VulkanMeshCache::VulkanMeshCache(VulkanGraphicsDevice& device, bool isDynamic) :
		_device(device),
		_isDynamic(isDynamic),
		_cachedMeshes()
	{
		// TODO: temporary
		static const uint64 meshBufferSize = 1024 * 1024 * 10; // 10MB

		BufferUsageFlags bufferFlags = BufferUsageFlags::Vertex | BufferUsageFlags::Index | BufferUsageFlags::TransferSource | BufferUsageFlags::TransferDestination;

		if (isDynamic)
		{
			bufferFlags |= BufferUsageFlags::HostVisible;
		}

		_meshBuffer = StaticRefCast<VulkanBuffer>(_device.CreateBuffer(meshBufferSize, bufferFlags));
	}

	VulkanMeshCache::~VulkanMeshCache()
	{
		Clear();
		_device.TryReleaseResource(_meshBuffer->ID);
	}

	const CachedVulkanMesh& VulkanMeshCache::CreateOrUpdateMesh(const SharedRef<Mesh>& mesh)
	{
		CocoAssert(mesh, "Mesh was null")

		const ResourceID& meshID = mesh->GetID();
		auto it = _cachedMeshes.find(meshID);

		CachedVulkanMesh* cachedMesh = nullptr;

		if (it == _cachedMeshes.end())
		{
			// Create a new entry for the mesh
			cachedMesh = &_cachedMeshes.try_emplace(meshID, mesh).first->second;
		}
		else
		{
			cachedMesh = &it->second;
			cachedMesh->Update(*mesh);
		}

		CocoAssert(cachedMesh, "Cached mesh was null")

		Ref<Buffer> stagingBuffer;

		try
		{
			uint64 vertexDataSize, indexDataSize;
			mesh->GetDataSizeRequirements(vertexDataSize, indexDataSize);

			uint64 totalSize = vertexDataSize + indexDataSize;

			// Make sure we have valid vertex and index blocks
			EnsureCachedDataIsAllocated(totalSize, *cachedMesh);

			cachedMesh->Buffer = _meshBuffer;
			cachedMesh->IndexBufferOffset = vertexDataSize;

			if (totalSize > 0)
			{
				uint64 bufferOffset = 0;
				Ref<Buffer> buffer;

				if (!_isDynamic)
				{
					// Use a staging buffer for transfering device-local data
					stagingBuffer = Buffer::CreateStaging(_device, totalSize);
					buffer = stagingBuffer;
				}
				else
				{
					buffer = _meshBuffer;
					bufferOffset = cachedMesh->AllocBlock->Offset;
				}

				// Load mesh data
				char* bufferMemory = (char*)buffer->GetMappedData();
				mesh->UploadData(bufferMemory + bufferOffset, vertexDataSize, bufferMemory + bufferOffset + cachedMesh->IndexBufferOffset, indexDataSize);

				if (stagingBuffer)
				{
					// Copy from the staging buffer
					stagingBuffer->CopyTo(0, *_meshBuffer, cachedMesh->AllocBlock->Offset, cachedMesh->AllocBlock->Size);
				}
			}

			cachedMesh->Use();
		}
		catch (const Exception& ex)
		{
			FreeCachedMeshData(*cachedMesh);

			CocoError("Update CachedMesh error: {}", ex.what())
		}

		if (stagingBuffer)
		{
			_device.TryReleaseResource(stagingBuffer->ID);
		}

		return *cachedMesh;
	}

	bool VulkanMeshCache::HasMesh(const ResourceID& meshID) const
	{
		return _cachedMeshes.contains(meshID);
	}

	const CachedVulkanMesh& VulkanMeshCache::GetMesh(const ResourceID& meshID)
	{
		CachedVulkanMesh& cachedMesh = _cachedMeshes.at(meshID);
		cachedMesh.Use();
		return cachedMesh;
	}

	bool VulkanMeshCache::TryGetMesh(const ResourceID& meshID, const CachedVulkanMesh*& outCachedMesh)
	{
		auto it = _cachedMeshes.find(meshID);

		if (it == _cachedMeshes.end())
			return false;

		CachedVulkanMesh& cachedMesh = it->second;
		cachedMesh.Use();

		outCachedMesh = &cachedMesh;
		return true;
	}

	void VulkanMeshCache::RemoveMesh(const ResourceID& meshID)
	{
		auto it = _cachedMeshes.find(meshID);

		if (it == _cachedMeshes.end())
			return;

		FreeCachedMeshData(it->second);
		_cachedMeshes.erase(it);
	}

	void VulkanMeshCache::Clear()
	{
		_meshBuffer->VirtualFreeAll();
		_cachedMeshes.clear();
	}

	uint64 VulkanMeshCache::PurgeUnusedMeshDatas()
	{
		uint64 purgeCount = 0;

		auto it = _cachedMeshes.begin();

		while (it != _cachedMeshes.end())
		{
			CachedVulkanMesh& cachedMesh = it->second;
			
			if (cachedMesh.MeshRef.expired())
			{
				FreeCachedMeshData(cachedMesh);
				it = _cachedMeshes.erase(it);
				purgeCount++;
			}
			else
			{
				++it;
			}
		}

		return purgeCount;
	}

	void VulkanMeshCache::CopyMesh(const CachedVulkanMesh& sourceMesh)
	{
		const ResourceID& meshID = sourceMesh.MeshID;
		auto it = _cachedMeshes.find(meshID);

		CachedVulkanMesh* cachedMesh = nullptr;

		if (it == _cachedMeshes.end())
		{
			// Create a new entry for the mesh
			cachedMesh = &_cachedMeshes.try_emplace(meshID, sourceMesh).first->second;
		}
		else
		{
			cachedMesh = &it->second;
			cachedMesh->Update(sourceMesh);
		}

		CocoAssert(cachedMesh, "Cached mesh was null")
		CocoAssert(sourceMesh.AllocBlock.has_value(), "Source mesh was not allocated")
		CocoAssert(sourceMesh.Buffer.IsValid(), "Source mesh buffer was invalid")
		
		try
		{
			// Make sure we have valid vertex and index blocks
			EnsureCachedDataIsAllocated(sourceMesh.AllocBlock->Size, *cachedMesh);
			cachedMesh->Buffer = _meshBuffer;

			// Copy from the original buffer
			sourceMesh.Buffer->CopyTo(sourceMesh.AllocBlock->Offset, *_meshBuffer, cachedMesh->AllocBlock->Offset, cachedMesh->AllocBlock->Size);
		}
		catch (const Exception& ex)
		{
			FreeCachedMeshData(*cachedMesh);

			CocoError("Copy CachedMesh error: {}", ex.what())
		}
	}

	void VulkanMeshCache::EnsureCachedDataIsAllocated(uint64 totalDataSize, CachedVulkanMesh& cachedMesh)
	{
		if (cachedMesh.AllocBlock.has_value() && cachedMesh.AllocBlock->Size != totalDataSize)
			FreeCachedMeshData(cachedMesh);

		if (!cachedMesh.AllocBlock.has_value() && totalDataSize > 0)
		{
			if (!_meshBuffer->VirtualAllocate(totalDataSize, cachedMesh.AllocBlock.emplace()))
			{
				throw Exception(FormatString("Could not allocate {} bytes for mesh data", totalDataSize));
			}
		}
	}

	void VulkanMeshCache::FreeCachedMeshData(CachedVulkanMesh& cachedMesh)
	{
		if (!cachedMesh.AllocBlock.has_value())
			return;

		_meshBuffer->VirtualFree(cachedMesh.AllocBlock.value());
		cachedMesh.AllocBlock.reset();
	}
}