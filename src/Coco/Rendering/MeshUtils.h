//
// Created by cullen on 3/31/26.
//

#ifndef COCOENGINE_MESHUTILS_H
#define COCOENGINE_MESHUTILS_H
#include "Mesh.h"
#include "Coco/Core/Math/Vector2.h"
#include "Coco/Core/Math/Vector3.h"
#include "Coco/Core/Types/ArrayContainer.h"

namespace Coco
{
    class MeshUtils
    {
    public:
        static void CreateXYGrid(
            const Vector2& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateXYGrid(
            const Vector2& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateXZGrid(
            const Vector2& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateXZGrid(
            const Vector2& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateZYGrid(
            const Vector2& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateZYGrid(
            const Vector2& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateCube(
            const Vector3& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateCube(
            const Vector3& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        static void CreateXYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            bool flipDirection = false);

        static void CreateXYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            Mesh& mesh,
            VertexChannelFlags channels,
            bool flipDirection = false);

        static void CreateXZEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            bool flipDirection = false);

        static void CreateXZEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            Mesh& mesh,
            VertexChannelFlags channels,
            bool flipDirection = false);

        static void CreateZYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            bool flipDirection = false);

        static void CreateZYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            Mesh& mesh,
            VertexChannelFlags channels,
            bool flipDirection = false);

    private:
        static uint32 CoordsToIndex(uint32 x, uint32 y, uint32 length);
    };
} // Coco

#endif //COCOENGINE_MESHUTILS_H