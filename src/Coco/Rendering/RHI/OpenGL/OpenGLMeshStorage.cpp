//
// Created by cullen on 3/14/26.
//

#include "OpenGLMeshStorage.h"
#include "Resources/OpenGLBuffer.h"
#include "OpenGLGraphicsPlatform.h"

namespace Coco
{
    OpenGLMeshBuffer::OpenGLMeshBuffer(Ref<OpenGLBuffer> meshBuffer) :
        MeshBuffer(meshBuffer),
        ChannelOffsets({0, 0, 0, 0, 0}),
        VertexChannels(VertexChannelFlags::None),
        VertexDataSize(0),
        IndexCount(0),
        VertexArrayHandle(0)
    {}

    OpenGLMeshBuffer::~OpenGLMeshBuffer()
    {
        if (VertexArrayHandle)
            glDeleteVertexArrays(1, &VertexArrayHandle);
    }

    void OpenGLMeshBuffer::UpdateVertexArray()
    {
        if (VertexArrayHandle == 0)
            glCreateVertexArrays(1, &VertexArrayHandle);

        glBindVertexArray(VertexArrayHandle);
        glBindBuffer(GL_ARRAY_BUFFER, MeshBuffer->GetHandle());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MeshBuffer->GetHandle());

        GLint layout = 0;
        for (uint8 channel = 0; channel < static_cast<uint8>(ChannelOffsets.GetCount()); ++channel)
        {
            VertexChannelFlags channelFlag = static_cast<VertexChannelFlags>(1 << channel);
            if ((channelFlag & VertexChannels) != channelFlag)
                continue;

            uint8 channelElementCount = GetVertexChannelElementCount(static_cast<VertexChannel>(channel));
            glVertexAttribPointer(layout, channelElementCount, GL_FLOAT, GL_FALSE, static_cast<GLint>(channelElementCount * sizeof(float)), reinterpret_cast<void*>(ChannelOffsets[channel]));
            glEnableVertexAttribArray(layout);
            layout++;
        }
    }

    OpenGLMeshStorage::OpenGLMeshStorage(OpenGLGraphicsPlatform* platform) :
        _platform(platform),
        _staticMeshBuffers(),
        _frameMeshBuffers(nullptr, 2)
    {}

    OpenGLMeshStorage::~OpenGLMeshStorage()
    {
        for (uint8 i = 0; i < static_cast<uint8>(_frameMeshBuffers.GetCount()); ++i)
            ClearFrameMeshBuffers(i);

        _frameMeshBuffers.Clear();

        for (const auto& pair : _staticMeshBuffers)
            _platform->InvalidateResource(pair.second.MeshBuffer->GetID());

        _staticMeshBuffers.Clear();
    }

    void OpenGLMeshStorage::AddMesh(Mesh& mesh)
    {
        // TODO: per-frame mesh storage
        auto meshBuffers = GetMeshBuffers(mesh.IsDynamic());
        auto existing = meshBuffers->TryGetValue(mesh.GetID());

        bool needsUpdate = !existing || mesh.NeedsUpdate() || mesh.IsDynamic();

        if (!needsUpdate)
            return;

        Ref<OpenGLBuffer> meshBuffer;
        uint64 vertexDataSize = mesh.GetVertexDataSize();
        uint64 indexDataSize = mesh.GetIndexDataSize();
        uint64 indexOffset = Math::GetAlignmentOffset(vertexDataSize, alignof(uint32));
        uint64 totalDataSize = indexOffset + indexDataSize;

        if (existing)
        {
            meshBuffer = existing->MeshBuffer;
            if (meshBuffer->GetSize() < totalDataSize)
                meshBuffer->Resize(totalDataSize);
        }
        else
        {
            // TODO: for static meshes, this shouldn't end up being GL_STREAM_DRAW
            meshBuffer = _platform->CreateBuffer(totalDataSize, BufferUsageFlags::Vertex | BufferUsageFlags::Index | BufferUsageFlags::HostVisible).Downcast<OpenGLBuffer>();
            existing = &meshBuffers->Emplace(mesh.GetID(), meshBuffer);
        }

        for (uint8 channel = 0; channel < static_cast<uint8>(existing->ChannelOffsets.GetCount()); ++channel)
            existing->ChannelOffsets[channel] = mesh.GetVertexDataOffset(static_cast<VertexChannel>(channel));

        existing->VertexChannels = mesh.GetVertexChannels();
        existing->VertexDataSize = vertexDataSize;
        existing->IndexDataOffset = indexOffset;
        existing->IndexCount = mesh.GetIndexCount();

        void* mappedData = meshBuffer->GetMappedPtr();
        COCO_ASSERT(mappedData, "Unable to map buffer");

        uint8* vertexDataPtr = static_cast<uint8*>(mappedData);
        uint8* indexDataPtr = vertexDataPtr + indexOffset;
        mesh.UpdateData(vertexDataPtr, indexDataPtr);

        existing->UpdateVertexArray();
    }

    void OpenGLMeshStorage::RemoveMesh(uint64 meshID)
    {
        if (auto* existing = _staticMeshBuffers.TryGetValue(meshID))
        {
            _platform->InvalidateResource(existing->MeshBuffer->GetID());
            _staticMeshBuffers.Remove(meshID);
        }
    }

    void OpenGLMeshStorage::ClearFrameMeshBuffers(uint8 frameIndex)
    {
        if (_frameMeshBuffers.GetCount() <= frameIndex)
            return;

        auto& meshBuffers = _frameMeshBuffers[frameIndex];
        if (meshBuffers.IsEmpty())
            return;

        for (const auto& meshBuffer : meshBuffers)
            _platform->InvalidateResource(meshBuffer.second.MeshBuffer->GetID());

        meshBuffers.Clear();
    }

    const OpenGLMeshBuffer* OpenGLMeshStorage::GetMeshBuffer(uint64 meshID) const
    {
        if (auto staticBuffer = _staticMeshBuffers.TryGetValue(meshID))
            return staticBuffer;

        uint8 frameIndex = _platform->GetCurrentFrameIndex();
        COCO_ASSERT(frameIndex < _frameMeshBuffers.GetCount(), "Dynamic mesh wasn't loaded in the frame it was required");
        return _frameMeshBuffers[frameIndex].TryGetValue(meshID);
    }

    Map<uint64, OpenGLMeshBuffer>* OpenGLMeshStorage::GetMeshBuffers(bool dynamic)
    {
        if (!dynamic)
            return &_staticMeshBuffers;

        uint8 frameIndex = _platform->GetCurrentFrameIndex();
        if (_frameMeshBuffers.GetCount() < frameIndex + 1)
            _frameMeshBuffers.Resize(frameIndex + 1);

        return &_frameMeshBuffers[frameIndex];
    }
} // Coco