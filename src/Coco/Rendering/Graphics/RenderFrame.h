//
// Created by cullen on 3/5/26.
//

#ifndef COCOENGINE_RENDERFRAME_H
#define COCOENGINE_RENDERFRAME_H
#include "RenderFrameStats.h"

#include "Coco/Core/Math/Matrix4x4.h"

#include "Resources/GraphicsSurface.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Memory/Allocators/FreeListAllocator.h"
#include "Coco/Rendering/RenderObjectView.h"
#include "Coco/Rendering/RenderSceneStorage.h"
#include "Coco/Rendering/RenderSceneTypes.h"

namespace Coco
{
    class MeshStorage;
    class Mesh;
    class RenderGraph;

    class RenderFrame
    {
        friend class RenderScene;
        friend class RenderObjectView;
        friend class RenderObjectView::Iterator;

    public:
        static constexpr uint8 AllocatorGroup = 10;

        virtual ~RenderFrame() = default;

        virtual void Render(RenderGraph&& graph, RenderScene&& scene, Ref<GraphicsSurface> surface) = 0;
        virtual Matrix4x4 CreateOrthographicProjection(float left, float right, float bottom, float top, float nearClip, float farClip) const = 0;
        virtual Matrix4x4 CreatePerspectiveProjection(float verticalFOV, float aspectRatio, float nearClip, float farClip) const = 0;

        virtual void NewFrame();

        RenderScene CreateRenderScene(const Sizei& frameSize);
        void EnsureMeshData(Mesh& mesh);
        void EnsureDynamicMeshData(uint64 id, Span<const Vector3> positions, Span<const uint32> indices,
            Optional<Span<const Vector3>> normals, Optional<Span<const Vector4>> colors,
            Optional<Span<const Vector4>> tangents, Optional<Span<const Vector2>> uvs);
        RenderSceneStorage& GetSceneStorage() { return _renderSceneStorage; }
        const RenderSceneStorage& GetSceneStorage() const { return _renderSceneStorage; }
        Allocator& GetFrameAllocator() { return _frameAllocator; }
        void AddDrawCall(uint32 triangleCount, uint32 vertexCount);
        RenderFrameStats GetStats() const;

    protected:
        static constexpr uint64 _sceneStoragePageSize = 1024 * 1024;
        static constexpr uint64 _sceneStorageUniformPageSize = 1024;
        static constexpr uint64 _renderObjectCount = 1024;
        static constexpr uint64 _frameAllocatorSize = 1024 * 1024 * 3 + sizeof(RenderObject) * _renderObjectCount;

        FreeListAllocator _frameAllocator;
        MeshStorage* _meshStorage;
        RenderSceneStorage _renderSceneStorage;
        uint64 _rendersThisFrame;
        Array<RenderObject> _renderObjects;
        RenderFrameStats _stats;

    protected:
        RenderFrame(MeshStorage* meshStorage);
    };
} // Coco

#endif //COCOENGINE_RENDERFRAME_H