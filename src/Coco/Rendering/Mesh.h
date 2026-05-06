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

    struct Submesh
    {
        uint32 IndexOffset;
        uint32 IndexCount;
        int32 VertexOffset;

        Submesh(uint32 indexOffset, uint32 indexCount, int32 vertexOffset = 0);
    };

    class Mesh : public Resource
    {
        DECLARE_RTTI_TYPE(Mesh);

    public:
        Mesh(Engine* engine, uint64 id, bool isDynamic = false);
        ~Mesh();

        void AppendVertex(const Vector3& position, const Vector3* normal = nullptr, const Vector4* tangent = nullptr, const Vector4* color = nullptr, const Vector2* uv = nullptr);
        void AppendVertices(Span<const Vector3> positions, Span<const Vector3>* normals = nullptr, Span<const Vector4>* tangents = nullptr, Span<const Vector4>* colors = nullptr, Span<const Vector2>* uvs = nullptr);
        void AppendIndices(Span<const uint32> indices);

        void SetPositions(Span<const Vector3> positions);
        Span<const Vector3> GetPositions() const { return _positions; }

        void SetNormals(Span<const Vector3> normals);
        Span<const Vector3> GetNormals() const { return _normals; }

        void SetTangents(Span<const Vector4> tangents);
        Span<const Vector4> GetTangents() const { return _tangents; }

        void SetColors(Span<const Vector4> colors);
        Span<const Vector4> GetColors() const { return _colors; }

        void SetUVs(Span<const Vector2> uvs);
        Span<const Vector2> GetUVs() const { return _uvs; }

        void SetIndices(Span<const uint32> indices);
        Span<const uint32> GetIndices() const { return _indices; }

        void SetSubmeshes(Span<const Submesh> submeshes);
        Span<const Submesh> GetSubmeshes() const { return _submeshes; }

        uint64 GetVertexCount() const { return _positions.GetCount(); }
        uint64 GetVertexDataSize() const;
        VertexChannelFlags GetVertexChannels() const { return _channels; }
        uint64 GetVertexDataOffset(VertexChannel channel) const;
        uint64 GetIndexDataSize() const;
        uint64 GetIndexCount() const { return _indices.GetCount(); }
        uint64 GetTotalDataSize() const;
        void CalculateTangents();

        bool IsDynamic() const { return _isDynamic; }
        bool NeedsUpdate() const { return _isDirty; }
        void UpdateData(void* vertexDataDestination, void* indexDataDestination, ArrayContainer<uint64>& outChannelDataOffsets);
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

        void MarkDirty();
    };
} // Coco

#endif //COCOENGINE_MESH_H