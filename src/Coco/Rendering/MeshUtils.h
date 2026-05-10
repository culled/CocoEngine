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
    /// @brief Helper functions for creating mesh primitives
    class MeshUtils
    {
    public:
        /// @brief Creates a plane mesh oriented along the X and Y axes, facing the +Z axis
        /// @param size The size of the plane in each axis
        /// @param offset The position of the plane's center
        /// @param outVertexPositions Will be filled with the vertex positions
        /// @param outIndices Will be filled with the vertex indices
        /// @param outVertexNormals If given, will be filled with the vertex normals
        /// @param outVertexUVs If given, will be filled with the vertex UVs
        /// @param subdivisions The number of times to subdivide the plane
        /// @param flipDirection If true, the plane will face the -Z axis
        static void CreateXYGrid(
            const Vector2& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a plane mesh oriented along the X and Y axes, facing the +Z axis
        /// @param size The size of the plane in each axis
        /// @param offset The position of the plane's center
        /// @param mesh The mesh
        /// @param channels The vertex channels to create data for
        /// @param subdivisions The number of times to subdivide the plane
        /// @param flipDirection If true, the plane will face the -Z axis
        static void CreateXYGrid(
            const Vector2& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a plane mesh oriented along the X and Z axes, facing the +Y axis
        /// @param size The size of the plane in each axis
        /// @param offset The position of the plane's center
        /// @param outVertexPositions Will be filled with the vertex positions
        /// @param outIndices Will be filled with the vertex indices
        /// @param outVertexNormals If given, will be filled with the vertex normals
        /// @param outVertexUVs If given, will be filled with the vertex UVs
        /// @param subdivisions The number of times to subdivide the plane
        /// @param flipDirection If true, the plane will face the -Y axis
        static void CreateXZGrid(
            const Vector2& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a plane mesh oriented along the X and Z axes, facing the +Y axis
        /// @param size The size of the plane in each axis
        /// @param offset The position of the plane's center
        /// @param mesh The mesh
        /// @param channels The vertex channels to create data for
        /// @param subdivisions The number of times to subdivide the plane
        /// @param flipDirection If true, the plane will face the -Y axis
        static void CreateXZGrid(
            const Vector2& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a plane mesh oriented along the Z and Y axes, facing the +X axis
        /// @param size The size of the plane in each axis
        /// @param offset The position of the plane's center
        /// @param outVertexPositions Will be filled with the vertex positions
        /// @param outIndices Will be filled with the vertex indices
        /// @param outVertexNormals If given, will be filled with the vertex normals
        /// @param outVertexUVs If given, will be filled with the vertex UVs
        /// @param subdivisions The number of times to subdivide the plane
        /// @param flipDirection If true, the plane will face the -X axis
        static void CreateZYGrid(
            const Vector2& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a plane mesh oriented along the Z and Y axes, facing the +X axis
        /// @param size The size of the plane in each axis
        /// @param offset The position of the plane's center
        /// @param mesh The mesh
        /// @param channels The vertex channels to create data for
        /// @param subdivisions The number of times to subdivide the plane
        /// @param flipDirection If true, the plane will face the -X axis
        static void CreateZYGrid(
            const Vector2& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a cube mesh
        /// @param size The size of the cube in each axis
        /// @param offset The position of the box's center
        /// @param outVertexPositions Will be filled with the vertex positions
        /// @param outIndices Will be filled with the vertex indices
        /// @param outVertexNormals If given, will be filled with the vertex normals
        /// @param outVertexUVs If given, will be filled with the vertex UVs
        /// @param subdivisions The number of times to subdivide the cube
        /// @param flipDirection If true, the box's faces will face inside it instead of outside it
        static void CreateCube(
            const Vector3& size,
            const Vector3& offset,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a cube mesh
        /// @param size The size of the cube in each axis
        /// @param offset The position of the box's center
        /// @param mesh The mesh
        /// @param channels The vertex channels to create data for
        /// @param subdivisions The number of times to subdivide the cube
        /// @param flipDirection If true, the box's faces will face inside it instead of outside it
        static void CreateCube(
            const Vector3& size,
            const Vector3& offset,
            Mesh& mesh,
            VertexChannelFlags channels,
            uint32 subdivisions = 0,
            bool flipDirection = false);

        /// @brief Creates a filled ellipse oriented along the X and Y axis, facing the +Z axis
        /// @param radii The radius of the ellipse along each axis
        /// @param offset The position of the ellipse's center
        /// @param vertices The number of vertices along the edge of the ellipse
        /// @param outVertexPositions Will be filled with the vertex positions
        /// @param outIndices Will be filled with the vertex indices
        /// @param outVertexNormals If given, will be filled with the vertex normals
        /// @param outVertexUVs If given, will be filled with the vertex UVs
        /// @param flipDirection If true, the ellipse will face along the -Z axis
        static void CreateXYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            bool flipDirection = false);

        /// @brief Creates a filled ellipse oriented along the X and Y axis, facing the +Z axis
        /// @param radii The radius of the ellipse along each axis
        /// @param offset The position of the ellipse's center
        /// @param vertices The number of vertices along the edge of the ellipse
        /// @param mesh The mesh
        /// @param channels The vertex channels to create data for
        /// @param flipDirection If true, the ellipse will face along the -Z axis
        static void CreateXYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            Mesh& mesh,
            VertexChannelFlags channels,
            bool flipDirection = false);

        /// @brief Creates a filled ellipse oriented along the X and Z axis, facing the +Y axis
        /// @param radii The radius of the ellipse along each axis
        /// @param offset The position of the ellipse's center
        /// @param vertices The number of vertices along the edge of the ellipse
        /// @param outVertexPositions Will be filled with the vertex positions
        /// @param outIndices Will be filled with the vertex indices
        /// @param outVertexNormals If given, will be filled with the vertex normals
        /// @param outVertexUVs If given, will be filled with the vertex UVs
        /// @param flipDirection If true, the ellipse will face along the -Y axis
        static void CreateXZEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            bool flipDirection = false);

        /// @brief Creates a filled ellipse oriented along the X and Z axis, facing the +Y axis
        /// @param radii The radius of the ellipse along each axis
        /// @param offset The position of the ellipse's center
        /// @param vertices The number of vertices along the edge of the ellipse
        /// @param mesh The mesh
        /// @param channels The vertex channels to create data for
        /// @param flipDirection If true, the ellipse will face along the -Y axis
        static void CreateXZEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            Mesh& mesh,
            VertexChannelFlags channels,
            bool flipDirection = false);

        /// @brief Creates a filled ellipse oriented along the Z and Y axis, facing the +X axis
        /// @param radii The radius of the ellipse along each axis
        /// @param offset The position of the ellipse's center
        /// @param vertices The number of vertices along the edge of the ellipse
        /// @param outVertexPositions Will be filled with the vertex positions
        /// @param outIndices Will be filled with the vertex indices
        /// @param outVertexNormals If given, will be filled with the vertex normals
        /// @param outVertexUVs If given, will be filled with the vertex UVs
        /// @param flipDirection If true, the ellipse will face along the -X axis
        static void CreateZYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            ArrayContainer<Vector3>& outVertexPositions,
            ArrayContainer<uint32>& outIndices,
            ArrayContainer<Vector3>* outVertexNormals = nullptr,
            ArrayContainer<Vector2>* outVertexUVs = nullptr,
            bool flipDirection = false);

        /// @brief Creates a filled ellipse oriented along the Z and Y axis, facing the +X axis
        /// @param radii The radius of the ellipse along each axis
        /// @param offset The position of the ellipse's center
        /// @param vertices The number of vertices along the edge of the ellipse
        /// @param mesh The mesh
        /// @param channels The vertex channels to create data for
        /// @param flipDirection If true, the ellipse will face along the -X axis
        static void CreateZYEllipse(
            const Vector2& radii,
            const Vector3& offset,
            uint32 vertices,
            Mesh& mesh,
            VertexChannelFlags channels,
            bool flipDirection = false);

    private:
        /// @brief Converts a grid coordinate into an index
        /// @param x The x index
        /// @param y The y index
        /// @param length The length of the X axis
        /// @return The index
        static uint32 CoordsToIndex(uint32 x, uint32 y, uint32 length);
    };
} // Coco

#endif //COCOENGINE_MESHUTILS_H