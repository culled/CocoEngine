//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_MESH_H
#define COCOENGINE_MESH_H
#include "Coco/Core/Math/Vector2.h"
#include "Coco/Core/Math/Vector3.h"
#include "Coco/Core/Math/Vector4.h"
#include "Coco/Core/Resources/Resource.h"
#include "Coco/Core/Types/Array.h"
#include "Graphics/VertexDataTypes.h"
#include "Graphics/Resources/Buffer.h"

namespace Coco
{
    class GraphicsPlatform;

    /// @brief A subsection of a mesh
    struct Submesh
    {
        /// @brief The offset of the first index in the index buffer
        uint32 IndexOffset;

        /// @brief The number of indices
        uint32 IndexCount;

        /// @brief An offset to apply to each vertex index
        int32 VertexOffset;

        Submesh(uint32 indexOffset, uint32 indexCount, int32 vertexOffset = 0);
    };

    /// @brief A Resource that defines mesh data
    class Mesh : public Resource
    {
        DECLARE_RTTI_TYPE(Mesh);

    public:
        Mesh(Engine* engine, uint64 id, bool isDynamic = false);
        ~Mesh();

        /// @brief Appends a vertex to this mesh
        /// @param position The vertex position
        /// @param normal The vertex normal (optional)
        /// @param tangent The vertex tangent (optional)
        /// @param color The vertex color (optional)
        /// @param uv The vertex UV (optional)
        void AppendVertex(const Vector3& position, const Vector3* normal = nullptr, const Vector4* tangent = nullptr, const Vector4* color = nullptr, const Vector2* uv = nullptr);

        /// @brief Appends vertices to this mesh
        /// @param positions The vertex positions
        /// @param normals The vertex normals (optional)
        /// @param tangents The vertex tangents (optional)
        /// @param colors The vertex colors (optional)
        /// @param uvs The vertex UVs (optional)
        void AppendVertices(
            Span<const Vector3> positions,
            Optional<Span<const Vector3>> normals = Optional<Span<const Vector3>>(),
            Optional<Span<const Vector4>> tangents = Optional<Span<const Vector4>>(),
            Optional<Span<const Vector4>> colors = Optional<Span<const Vector4>>(),
            Optional<Span<const Vector2>> uvs = Optional<Span<const Vector2>>()
        );

        /// @brief Appends indices to this mesh
        /// @param indices The indices
        void AppendIndices(Span<const uint32> indices);

        /// @brief Sets the positions of this mesh. NOTE: this also sets the number of vertices in this mesh
        /// @param positions The vertex positions
        void SetPositions(Span<const Vector3> positions);

        /// @brief Gets the vertex positions of this mesh
        /// @return The vertex positions
        Span<const Vector3> GetPositions() const { return _positions; }

        /// @brief Sets the vertex normals of this mesh
        /// @param normals The vertex normals
        void SetNormals(Span<const Vector3> normals);

        /// @brief Gets the vertex normals of this mesh
        /// @return The vertex normals
        Span<const Vector3> GetNormals() const { return _normals; }

        /// @brief Sets the vertex tangents of this mesh
        /// @param tangents The vertex tangents
        void SetTangents(Span<const Vector4> tangents);

        /// @brief Gets the vertex tangents of this mesh
        /// @return The vertex tangents
        Span<const Vector4> GetTangents() const { return _tangents; }

        /// @brief Sets the vertex colors of this mesh
        /// @param colors The vertex colors
        void SetColors(Span<const Vector4> colors);

        /// @brief Gets the vertex colors of this mesh
        /// @return The vertex colors
        Span<const Vector4> GetColors() const { return _colors; }

        /// @brief Sets the vertex UVs of this mesh
        /// @param uvs The vertex UVs
        void SetUVs(Span<const Vector2> uvs);

        /// @brief Gets the vertex UVs of this mesh
        /// @return The vertex UVs
        Span<const Vector2> GetUVs() const { return _uvs; }

        /// @brief Sets the vertex indices of this mesh. Triangles are built by indices into the vertex buffer
        /// @param indices The vertex indices
        void SetIndices(Span<const uint32> indices);

        /// @brief Gets the vertex indices of this mesh
        /// @return The vertex indices
        Span<const uint32> GetIndices() const { return _indices; }

        /// @brief Sets the submeshes of this mesh. Each submesh represents a portion of the vertices to draw. Meshes always have at least one submesh
        /// @param submeshes The submeshes
        void SetSubmeshes(Span<const Submesh> submeshes);

        /// @brief Gets the submeshes of this mesh
        /// @return The submeshes
        Span<const Submesh> GetSubmeshes() const { return _submeshes; }

        /// @brief Gets the number of vertices in this mesh
        /// @return The number of vertices
        uint64 GetVertexCount() const { return _positions.GetCount(); }

        /// @brief Gets the total size of the buffer needed to hold the vertex data of this mesh
        /// @return The total vertex buffer size, in bytes
        uint64 GetVertexDataSize() const;

        /// @brief Gets the channels of vertex data stored by this mesh
        /// @return The vertex channels of this mesh
        VertexChannelFlags GetVertexChannels() const { return _channels; }

        /// @brief Gets the offset within the vertex buffer for a specific channel's data. Channel datas are packed contiguously
        /// @param channel The vertex channel
        /// @return The offset in the vertex buffer for the channel's specific data, in bytes
        uint64 GetVertexDataOffset(VertexChannel channel) const;

        /// @brief Gets the total size of the buffer needed to hold the index buffer of this mesh
        /// @return The total index buffer size, in bytes
        uint64 GetIndexDataSize() const;

        /// @brief Gets the number of indices in this mesh
        /// @return The number of indices
        uint64 GetIndexCount() const { return _indices.GetCount(); }

        /// @brief Gets the total size of the buffer needed to hold all of this mesh's data
        /// @return The total buffer size, in bytes
        uint64 GetTotalDataSize() const;

        /// @brief Calculates tangents for this mesh. This requires the mesh to have normals and UVs
        void CalculateTangents();

        /// @brief Gets the dynamic state of this mesh. Dynamic meshes are designed to be updated every frame
        /// @return True if this mesh is dynamic
        bool IsDynamic() const { return _isDynamic; }

        /// @brief Determines if this mesh's data needs to be updated on the GPU
        /// @return True if this mesh's data needs to be updated on the GPU
        bool NeedsUpdate() const { return _isDirty; }

        /// @brief Updates the data from this mesh to pointers into a vertex and index buffer
        /// @param vertexDataDestination A pointer to the vertex buffer where the vertex data will be stored
        /// @param indexDataDestination A pointer to the index buffer where the index buffer will be stored
        /// @param outChannelDataOffsets Will be filled with the offsets into the vertex buffer where each channel's data is stored
        void UpdateData(void* vertexDataDestination, void* indexDataDestination, ArrayContainer<uint64>& outChannelDataOffsets);

        /// @brief Clears all vertex and index data in this mesh
        void Clear();

    private:
        bool _isDirty;
        bool _isDynamic;
        VertexChannelFlags _channels;
        Array<Vector3> _positions;
        Array<Vector3> _normals;
        Array<Vector4> _tangents;
        Array<Vector4> _colors;
        Array<Vector2> _uvs;
        Array<uint32> _indices;
        Array<Submesh> _submeshes;

        /// @brief Marks this mesh as needing to be updated
        void MarkDirty();
    };
} // Coco

#endif //COCOENGINE_MESH_H