//
// Created by cullen on 3/5/26.
//

#include "RenderFrame.h"

#include "MeshStorage.h"
#include "Coco/Rendering/RenderScene.h"

namespace Coco
{
    void RenderFrame::NewFrame()
    {
        _renderSceneStorage.Clear();
        _rendersThisFrame = 0;
        _renderObjects.Clear();
        _stats = RenderFrameStats();
    }

    RenderScene RenderFrame::CreateRenderScene(const Sizei& frameSize)
    {
        return {this, _rendersThisFrame++, frameSize};
    }

    void RenderFrame::EnsureMeshData(Mesh& mesh)
    {
        _meshStorage->AddMesh(mesh);
    }

    void RenderFrame::EnsureDynamicMeshData(uint64 id, Span<const Vector3> positions, Span<const uint32> indices,
        Optional<Span<const Vector3>> normals, Optional<Span<const Vector4>> colors,
        Optional<Span<const Vector4>> tangents, Optional<Span<const Vector2>> uvs)
    {
        _meshStorage->AddDynamicMeshData(id, positions, indices, normals, colors, tangents, uvs);
    }

    void RenderFrame::AddDrawCall(uint32 triangleCount, uint32 vertexCount)
    {
        _stats.DrawCalls++;
        _stats.TrianglesDrawn += triangleCount;
        _stats.VerticesDrawn += vertexCount;
    }

    RenderFrameStats RenderFrame::GetStats() const
    {
        RenderFrameStats stats(_stats);
        stats.MemoryUsage = _frameAllocator.GetUsage();
        return stats;
    }

    RenderFrame::RenderFrame(MeshStorage* meshStorage) :
        _frameAllocator(AllocatorGroup, _frameAllocatorSize),
        _meshStorage(meshStorage),
        _renderSceneStorage(&_frameAllocator, _sceneStoragePageSize, _sceneStorageUniformPageSize),
        _rendersThisFrame(0),
        _renderObjects(&_frameAllocator, _renderObjectCount),
        _stats()
    {}
} // Coco