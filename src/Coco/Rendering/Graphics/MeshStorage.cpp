//
// Created by cullen on 3/22/26.
//
#include "MeshStorage.h"
#include "Resources/Buffer.h"
#include "PagedLinearBuffer.h"
#include "Coco/Rendering/Mesh.h"

namespace Coco
{
    MeshEntry::MeshEntry(const Mesh& mesh) :
        MeshBuffer(),
        BufferOffset(0),
        ChannelOffsets({0, 0, 0, 0, 0}),
        VertexDataSize(mesh.GetVertexDataSize()),
        IndexDataOffset(Math::GetAlignmentOffset(VertexDataSize, alignof(uint32))),
        IndexCount(mesh.GetIndexCount()),
        TotalDataSize(IndexDataOffset + IndexCount * sizeof(uint32))
    {}

    MeshEntry::MeshEntry(uint64 vertexDataSize, uint64 indexCount) :
        MeshBuffer(),
        BufferOffset(0),
        ChannelOffsets({0, 0, 0, 0, 0}),
        VertexDataSize(vertexDataSize),
        IndexDataOffset(Math::GetAlignmentOffset(VertexDataSize, alignof(uint32))),
        IndexCount(indexCount),
        TotalDataSize(IndexDataOffset + IndexCount * sizeof(uint32))
        {}

    MeshStorage::MeshStorage(GraphicsPlatform* platform, uint8 dynamicBufferCount) :
        _platform(platform),
        _staticMeshes(),
        _dynamicMeshes(),
        _dynamicMeshBuffers(nullptr, dynamicBufferCount),
        _currentDynamicMeshBufferIndex(0)
    {
        SetDynamicMeshBufferCount(dynamicBufferCount);
    }

    MeshStorage::~MeshStorage()
    {
        _dynamicMeshes.Clear();
        _dynamicMeshBuffers.Clear(true);

        for (auto& staticMesh : _staticMeshes)
            _platform->InvalidateResource(staticMesh.second.MeshBuffer->GetID());

        _staticMeshes.Clear();
    }

    void MeshStorage::AddMesh(Mesh& mesh)
    {
        if (mesh.IsDynamic())
            AddDynamicMesh(mesh);
        else
            AddStaticMesh(mesh);
    }

    void MeshStorage::AddDynamicMeshData(uint64 meshID, Span<const Vector3> positions, Span<const uint32> indices,
        Optional<Span<const Vector3>> normals, Optional<Span<const Vector4>> colors,
        Optional<Span<const Vector4>> tangents, Optional<Span<const Vector2>> uvs)
    {
        if (_dynamicMeshes.Contains(meshID))
            return;

        uint64 vertexDataSize = positions.size() * sizeof(Vector3);

        if (normals)
            vertexDataSize += normals->size() * sizeof(Vector3);

        if (colors)
            vertexDataSize += colors->size() * sizeof(Vector4);

        if (tangents)
            vertexDataSize += tangents->size() * sizeof(Vector4);

        if (uvs)
            vertexDataSize += uvs->size() * sizeof(Vector2);

        auto& entry = _dynamicMeshes.Emplace(meshID, vertexDataSize, indices.size());
        uint64 currentOffset = positions.size() * sizeof(Vector3);

        if (normals)
        {
            entry.ChannelOffsets[static_cast<uint8>(VertexChannel::Normal)] = currentOffset;
            currentOffset += normals->size() * sizeof(Vector3);
        }

        if (tangents)
        {
            entry.ChannelOffsets[static_cast<uint8>(VertexChannel::Tangent)] = currentOffset;
            currentOffset += tangents->size() * sizeof(Vector4);
        }

        if (colors)
        {
            entry.ChannelOffsets[static_cast<uint8>(VertexChannel::Color)] = currentOffset;
            currentOffset += colors->size() * sizeof(Vector4);
        }

        if (uvs)
        {
            entry.ChannelOffsets[static_cast<uint8>(VertexChannel::UV0)] = currentOffset;
            currentOffset += uvs->size() * sizeof(Vector2);
        }

        auto& dynamicBuffers = _dynamicMeshBuffers[_currentDynamicMeshBufferIndex];
        dynamicBuffers.Allocate(entry.TotalDataSize, entry.MeshBuffer, entry.BufferOffset);

        void* mappedData = entry.MeshBuffer->GetMappedPtr();
        COCO_ASSERT(mappedData, "Unable to map buffer");

        uint8* vertexDataPtr = static_cast<uint8*>(mappedData) + entry.BufferOffset;
        memcpy(vertexDataPtr, positions.data(), positions.size() * sizeof(Vector3));

        if (normals)
            memcpy(vertexDataPtr + entry.ChannelOffsets[static_cast<uint8>(VertexChannel::Normal)], normals->data(), normals->size() * sizeof(Vector3));

        if (tangents)
            memcpy(vertexDataPtr + entry.ChannelOffsets[static_cast<uint8>(VertexChannel::Tangent)], tangents->data(), tangents->size() * sizeof(Vector4));

        if (colors)
            memcpy(vertexDataPtr + entry.ChannelOffsets[static_cast<uint8>(VertexChannel::Color)], colors->data(), colors->size() * sizeof(Vector4));

        if (uvs)
            memcpy(vertexDataPtr + entry.ChannelOffsets[static_cast<uint8>(VertexChannel::UV0)], uvs->data(), uvs->size() * sizeof(Vector2));

        uint8* indexDataPtr = vertexDataPtr + entry.IndexDataOffset;
        memcpy(indexDataPtr, indices.data(), indices.size() * sizeof(uint32));
    }

    void MeshStorage::RemoveMesh(uint64 meshID)
    {
        if (auto entry = _staticMeshes.TryGetValue(meshID))
        {
            _platform->InvalidateResource(entry->MeshBuffer->GetID());
            _staticMeshes.Remove(meshID);
        }
    }

    const MeshEntry* MeshStorage::GetMesh(uint64 meshID) const
    {
        if (auto existing = _staticMeshes.TryGetValue(meshID))
            return existing;

        return _dynamicMeshes.TryGetValue(meshID);
    }

    void MeshStorage::SetDynamicMeshBufferCount(uint8 count)
    {
        BufferDescription desc(_bufferPageSize, BufferUsageFlags::Vertex | BufferUsageFlags::Index | BufferUsageFlags::HostVisible);
        _dynamicMeshBuffers.Resize(count, PagedLinearBuffer<Buffer>(_platform, desc, _platform->GetDeviceDescription().MinimumBufferAlignment));
    }

    void MeshStorage::SetCurrentDynamicMeshBuffer(uint8 index)
    {
        COCO_ASSERT(index < _dynamicMeshBuffers.GetCount(), "Invalid index");
        _currentDynamicMeshBufferIndex = index;
        _dynamicMeshBuffers[_currentDynamicMeshBufferIndex].Clear();
        _dynamicMeshes.Clear();
    }

    void MeshStorage::AddDynamicMesh(Mesh& mesh)
    {
        if (_dynamicMeshes.Contains(mesh.GetID()))
            return;

        auto& entry = _dynamicMeshes.Emplace(mesh.GetID(), mesh);
        auto& dynamicBuffers = _dynamicMeshBuffers[_currentDynamicMeshBufferIndex];
        dynamicBuffers.Allocate(entry.TotalDataSize, entry.MeshBuffer, entry.BufferOffset);

        void* mappedData = entry.MeshBuffer->GetMappedPtr();
        COCO_ASSERT(mappedData, "Unable to map buffer");

        uint8* vertexDataPtr = static_cast<uint8*>(mappedData) + entry.BufferOffset;
        uint8* indexDataPtr = vertexDataPtr + entry.IndexDataOffset;
        mesh.UpdateData(vertexDataPtr, indexDataPtr, entry.ChannelOffsets);
    }

    void MeshStorage::AddStaticMesh(Mesh& mesh)
    {
        MeshEntry* entry = _staticMeshes.TryGetValue(mesh.GetID());

        bool needsUpdate = !entry || mesh.NeedsUpdate();

        if (!needsUpdate)
            return;

        uint64 totalDataSize = mesh.GetTotalDataSize();

        if (entry)
        {
            if (entry->MeshBuffer->GetSize() < totalDataSize)
                entry->MeshBuffer->Resize(totalDataSize);
        }
        else
        {
            entry = &_staticMeshes.Emplace(mesh.GetID(), mesh);
            BufferDescription desc(entry->TotalDataSize, BufferUsageFlags::Vertex | BufferUsageFlags::Index | BufferUsageFlags::TransferDestination);

            entry->MeshBuffer = _platform->CreateBuffer(desc);
        }

        StagingOperation* stagingOperation = _platform->GetStagingBuffer()->CreateStagingOperation(totalDataSize);

        uint8* vertexDataPtr = stagingOperation->BufferPtr;
        uint8* indexDataPtr = vertexDataPtr + entry->IndexDataOffset;
        mesh.UpdateData(vertexDataPtr, indexDataPtr, entry->ChannelOffsets);

        entry->MeshBuffer->CopyFrom(*stagingOperation);
    }
}
