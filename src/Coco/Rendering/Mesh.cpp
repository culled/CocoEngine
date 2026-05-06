//
// Created by cullen on 3/10/26.
//

#include "Mesh.h"

#include <cstring>

#include "RenderService.h"
#include "Coco/Core/Engine.h"

namespace Coco
{
    Submesh::Submesh(uint32 indexOffset, uint32 indexCount, int32 vertexOffset) :
        IndexOffset(indexOffset),
        IndexCount(indexCount),
        VertexOffset(vertexOffset)
    {}
    
    DEFINE_RTTI_TYPE(Mesh, Resource);

    Mesh::Mesh(Engine* engine, uint64 id, bool isDynamic) :
        Resource(engine, id),
        _isDirty(false),
        _isDynamic(isDynamic),
        _channels(VertexChannelFlags::None),
        _positions(),
        _indices()
    {}

    Mesh::~Mesh()
    {
        if (!_isDynamic)
        {
            if (RenderService* rendering = _engine->GetService<RenderService>())
            {
                if (GraphicsPlatform* platform = rendering->GetGraphicsPlatform())
                    platform->GetMeshStorage()->RemoveMesh(GetID());
            }
        }

        _indices.Clear(true);
        _uvs.Clear(true);
        _colors.Clear(true);
        _tangents.Clear(true);
        _normals.Clear(true);
        _positions.Clear(true);
    }

    void Mesh::AppendVertex(const Vector3& position, const Vector3* normal, const Vector4* tangent,
        const Vector4* color, const Vector2* uv)
    {
        _positions.Append(position);

        // TODO: ensure all arrays match in size

        if (normal)
        {
            _normals.Append(*normal);
            _channels |= VertexChannelFlags::Normal;
        }

        if (tangent)
        {
            _tangents.Append(*tangent);
            _channels |= VertexChannelFlags::Tangent;
        }

        if (color)
        {
            _colors.Append(*color);
            _channels |= VertexChannelFlags::Color;
        }

        if (uv)
        {
            _uvs.Append(*uv);
            _channels |= VertexChannelFlags::UV0;
        }

        MarkDirty();
    }

    void Mesh::AppendVertices(Span<const Vector3> positions, Span<const Vector3>* normals,
        Span<const Vector4>* tangents, Span<const Vector4>* colors, Span<const Vector2>* uvs)
    {
        _positions.AppendRange(positions);

        // TODO: ensure all arrays match in size

        if (normals)
        {
            _normals.AppendRange(*normals);
            _channels |= VertexChannelFlags::Normal;
        }

        if (tangents)
        {
            _tangents.AppendRange(*tangents);
            _channels |= VertexChannelFlags::Tangent;
        }

        if (colors)
        {
            _colors.AppendRange(*colors);
            _channels |= VertexChannelFlags::Color;
        }

        if (uvs)
        {
            _uvs.AppendRange(*uvs);
            _channels |= VertexChannelFlags::UV0;
        }

        MarkDirty();
    }

    void Mesh::AppendIndices(Span<const uint32> indices)
    {
        _indices.AppendRange(indices);

        _submeshes.Clear();
        _submeshes.EmplaceBack(0, _indices.GetCount());

        MarkDirty();
    }

    void Mesh::SetPositions(Span<const Vector3> positions)
    {
        _positions.Set(positions);

        _channels |= VertexChannelFlags::Position;

        MarkDirty();
    }

    void Mesh::SetNormals(Span<const Vector3> normals)
    {
        COCO_ASSERT(normals.size() == _positions.GetCount(), "Normals and positions must match in size");

        _normals.Set(normals);

        _channels |= VertexChannelFlags::Normal;

        MarkDirty();
    }

    void Mesh::SetTangents(Span<const Vector4> tangents)
    {
        COCO_ASSERT(tangents.size() == _positions.GetCount(), "Tangents and positions must match in size");

        _tangents.Set(tangents);

        _channels |= VertexChannelFlags::Tangent;

        MarkDirty();
    }

    void Mesh::SetColors(Span<const Vector4> colors)
    {
        COCO_ASSERT(colors.size() == _positions.GetCount(), "Colors and positions must match in size");

        _colors.Set(colors);

        _channels |= VertexChannelFlags::Color;

        MarkDirty();
    }

    void Mesh::SetUVs(Span<const Vector2> uvs)
    {
        COCO_ASSERT(uvs.size() == _positions.GetCount(), "UVs and positions must match in size");

        _uvs.Set(uvs);

        _channels |= VertexChannelFlags::UV0;

        MarkDirty();
    }

    void Mesh::SetIndices(Span<const uint32> indices)
    {
        COCO_ASSERT(indices.size() % 3 == 0, "Indices must be a multiple of 3");

        _indices.Set(indices);

        _submeshes.Clear();
        _submeshes.EmplaceBack(0, indices.size());

        MarkDirty();
    }

    void Mesh::SetSubmeshes(Span<const Submesh> submeshes)
    {
        _submeshes.Clear();

        for (const auto& s : submeshes)
        {
            if (s.IndexOffset + s.IndexCount > _indices.GetCount())
            {
                throw Exception("Submesh is out of the range of indices");
            }

            _submeshes.Append(s);
        }
    }

    uint64 Mesh::GetVertexDataSize() const
    {
        uint64 size = _positions.GetCount() * sizeof(Vector3);

        if (!_normals.IsEmpty())
            size += _normals.GetCount() * sizeof(Vector3);

        if (!_tangents.IsEmpty())
            size += _tangents.GetCount() * sizeof(Vector4);

        if (!_colors.IsEmpty())
            size += _colors.GetCount() * sizeof(Vector4);

        if (!_uvs.IsEmpty())
            size += _uvs.GetCount() * sizeof(Vector2);

        return size;
    }

    uint64 Mesh::GetVertexDataOffset(VertexChannel channel) const
    {
        if (channel == VertexChannel::Position)
            return 0;

        uint64 offset = _positions.GetCount() * sizeof(Vector3);

        if (channel == VertexChannel::Normal)
            return offset;

        offset += _normals.GetCount() * sizeof(Vector3);

        if (channel == VertexChannel::Tangent)
            return offset;

        offset += _tangents.GetCount() * sizeof(Vector4);

        if (channel == VertexChannel::Color)
            return offset;

        offset += _colors.GetCount() * sizeof(Vector4);

        if (channel == VertexChannel::UV0)
            return offset;

        offset += _uvs.GetCount() * sizeof(Vector2);

        return offset;
    }

    uint64 Mesh::GetIndexDataSize() const
    {
        return sizeof(uint32) * _indices.GetCount();
    }

    uint64 Mesh::GetTotalDataSize() const
    {
        uint64 vertexDataSize = GetVertexDataSize();
        uint64 indexOffset = Math::GetAlignmentOffset(vertexDataSize, alignof(uint32));
        return indexOffset + GetIndexDataSize();
    }

    void Mesh::CalculateTangents()
    {
        if (_positions.IsEmpty())
        {
            COCO_ENGINE_LOG_ERROR("No positions have been set");
            return;
        }

        if (_normals.GetCount() != _positions.GetCount())
        {
            COCO_ENGINE_LOG_ERROR("Tangents require normals to have been set");
            return;
        }

        if (_uvs.GetCount() != _positions.GetCount())
        {
            COCO_ENGINE_LOG_ERROR("Tangents require UVs to have been set");
            return;
        }

        Map<uint64, Vector4> tangents;

        // https://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal
        for (uint64 i = 0; i < _indices.GetCount(); i += 3)
        {
            const uint32 i0 = _indices[i];
            const uint32 i1 = _indices[i + 1];
            const uint32 i2 = _indices[i + 2];

            Vector3 v1 = _positions[i1] - _positions[i0];
            Vector3 v2 = _positions[i2] - _positions[i0];

            Vector2 t1 = _uvs[i1] - _uvs[i0];
            Vector2 t2 = _uvs[i2] - _uvs[i0];

            Vector3 normal = (_normals[i0] + _normals[i1] + _normals[i2]).Normalized();

            v1 -= normal * v1.Dot(normal);
            v2 -= normal * v2.Dot(normal);

            float handedness = ((t1.X() * t2.Y() - t1.Y() * t2.X()) < 0.0f) ? -1.0f : 1.0f;
            Vector3 s = ((v1 * t2.Y() - v2 * t1.Y()) * handedness).Normalized();

            Vector4 tangent4(s, handedness);

            if (tangents.Contains(i0))
            {
                tangents[i0] += tangent4;
            }
            else
            {
                tangents.Emplace(i0, tangent4);
            }

            if (tangents.Contains(i1))
            {
                tangents[i1] += tangent4;
            }
            else
            {
                tangents.Emplace(i1, tangent4);
            }

            if (tangents.Contains(i2))
            {
                tangents[i2] += tangent4;
            }
            else
            {
                tangents.Emplace(i2, tangent4);
            }
        }

        _tangents.Resize(_positions.GetCount());

        for (uint64 i = 0; i < tangents.GetCount(); i++)
        {
            const Vector4& t = tangents.Get(i);
            Vector3 xyz(t.XYZ().Normalized());

            _tangents.At(i) = Vector4(xyz, t.W() > 0.0f ? 1.0f : -1.0f);
        }

        _channels |= VertexChannelFlags::Tangent;
    }

    void Mesh::UpdateData(void* vertexDataDestination, void* indexDataDestination, ArrayContainer<uint64>& outChannelDataOffsets)
    {
        uint64 currentOffset = _positions.GetCount() * sizeof(Vector3);
        uint8* dst = static_cast<uint8*>(vertexDataDestination);
        memcpy(dst, _positions.Data(), currentOffset);
        outChannelDataOffsets[static_cast<uint8>(VertexChannel::Position)] = 0;
        dst += currentOffset;

        if (!_normals.IsEmpty())
        {
            uint64 normalsSize = _normals.GetCount() * sizeof(Vector3);
            memcpy(dst, _normals.Data(), normalsSize);

            outChannelDataOffsets[static_cast<uint8>(VertexChannel::Normal)] = currentOffset;
            currentOffset += normalsSize;
            dst += normalsSize;
        }

        if (!_tangents.IsEmpty())
        {
            uint64 tangentsSize = _tangents.GetCount() * sizeof(Vector4);
            memcpy(dst, _tangents.Data(), tangentsSize);

            outChannelDataOffsets[static_cast<uint8>(VertexChannel::Tangent)] = currentOffset;
            currentOffset += tangentsSize;
            dst += tangentsSize;
        }

        if (!_colors.IsEmpty())
        {
            uint64 colorsSize = _colors.GetCount() * sizeof(Vector4);
            memcpy(dst, _colors.Data(), colorsSize);

            outChannelDataOffsets[static_cast<uint8>(VertexChannel::Color)] = currentOffset;
            currentOffset += colorsSize;
            dst += colorsSize;
        }

        if (!_uvs.IsEmpty())
        {
            uint64 uvSize = _uvs.GetCount() * sizeof(Vector2);
            memcpy(dst, _uvs.Data(), uvSize);

            outChannelDataOffsets[static_cast<uint8>(VertexChannel::UV0)] = currentOffset;
        }

        memcpy(indexDataDestination, _indices.Data(), sizeof(uint32) * _indices.GetCount());

        _isDirty = false;
    }

    void Mesh::Clear()
    {
        _positions.Clear();
        _normals.Clear();
        _tangents.Clear();
        _colors.Clear();
        _uvs.Clear();
        _indices.Clear();
        _submeshes.Clear();

        _channels = VertexChannelFlags::None;

        MarkDirty();
    }

    void Mesh::MarkDirty()
    {
        _isDirty = true;
    }
} // Coco