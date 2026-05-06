//
// Created by cullen on 3/14/26.
//

#ifndef COCOENGINE_MESHSTORAGE_H
#define COCOENGINE_MESHSTORAGE_H
#include <Coco/Core/Types/CoreTypes.h>

#include "VertexDataTypes.h"

#include "Coco/Core/Math/Vector2.h"
#include "Coco/Core/Math/Vector3.h"
#include "Coco/Core/Math/Vector4.h"
#include "Coco/Core/Memory/Ptrs.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/Map.h"
#include "Coco/Core/Types/Optional.h"
#include "Coco/Core/Types/StackArray.h"

namespace Coco
{
    class GraphicsPlatform;
    class Mesh;
    class Buffer;

    template<typename BufferType>
    class PagedLinearBuffer;

    struct MeshEntry
    {
        Ref<Buffer> MeshBuffer;
        uint64 BufferOffset;
        StackArray<uint64, 5> ChannelOffsets;
        uint64 VertexDataSize;
        uint64 IndexDataOffset;
        uint64 IndexCount;
        uint64 TotalDataSize;

        MeshEntry(const Mesh& mesh);
        MeshEntry(uint64 vertexDataSize, uint64 indexCount);
    };

    class MeshStorage
    {
    public:
        MeshStorage(GraphicsPlatform* platform, uint8 dynamicBufferCount);
        ~MeshStorage();

        void AddMesh(Mesh& mesh);
        void AddDynamicMeshData(uint64 meshID, Span<const Vector3> positions, Span<const uint32> indices,
            Optional<Span<const Vector3>> normals, Optional<Span<const Vector4>> colors,
            Optional<Span<const Vector4>> tangents, Optional<Span<const Vector2>> uvs);
        void RemoveMesh(uint64 meshID);

        const MeshEntry* GetMesh(uint64 meshID) const;

        void SetDynamicMeshBufferCount(uint8 count);
        void SetCurrentDynamicMeshBuffer(uint8 index);

    private:
        static constexpr int _bufferPageSize = 1024 * 1024;
        GraphicsPlatform* _platform;
        Map<uint64, MeshEntry> _staticMeshes;
        Map<uint64, MeshEntry> _dynamicMeshes;
        Array<PagedLinearBuffer<Buffer>> _dynamicMeshBuffers;
        uint8 _currentDynamicMeshBufferIndex;

    private:
        void AddDynamicMesh(Mesh& mesh);
        void AddStaticMesh(Mesh& mesh);
    };
} // Coco

#endif //COCOENGINE_MESHSTORAGE_H