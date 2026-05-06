//
// Created by cullen on 3/31/26.
//

#include "MeshUtils.h"

namespace Coco
{
    void MeshUtils::CreateXYGrid(const Vector2& size, const Vector3& offset,
        ArrayContainer<Vector3>& outVertexPositions, ArrayContainer<uint32>& outIndices,
        ArrayContainer<Vector3>* outVertexNormals, ArrayContainer<Vector2>* outVertexUVs, uint32 subdivisions,
        bool flipDirection)
    {
        const uint32 vertexSideCount = 2 + subdivisions;
        const float vertexScaling = 1.0f / (vertexSideCount - 1);

        uint32 totalVertexCount = vertexSideCount * vertexSideCount;
        outVertexPositions.Reserve(outVertexPositions.GetCount() + totalVertexCount);

        if (outVertexNormals)
            outVertexNormals->Reserve(outVertexNormals->GetCount() + totalVertexCount);

        if (outVertexUVs)
            outVertexUVs->Reserve(outVertexUVs->GetCount() + totalVertexCount);

        const uint32 indexOffset = static_cast<uint32>(outVertexPositions.GetCount());

        Vector3 normalDir = flipDirection ? Vector3::Forward : Vector3::Backward;

        for (uint32 x = 0; x < vertexSideCount; x++)
        {
            for (uint32 y = 0; y < vertexSideCount; y++)
            {
                const float u = static_cast<float>(x) / (vertexSideCount - 1);
                const float v = static_cast<float>(y) / (vertexSideCount - 1);

                outVertexPositions.EmplaceBack((x * vertexScaling - 0.5f) * size.X() + offset.X(), (y * vertexScaling - 0.5f) * size.Y() + offset.Y(), offset.Z());

                if (outVertexNormals)
                    outVertexNormals->Append(normalDir);

                if (outVertexUVs)
                    outVertexUVs->EmplaceBack(flipDirection ? 1.0f - u : u, v);
            }
        }

        outIndices.Reserve(outIndices.GetCount() + (vertexSideCount - 1) * (vertexSideCount - 1) * 6);

        for (uint32 x = 0; x < vertexSideCount - 1; x++)
        {
            for (uint32 y = 0; y < vertexSideCount - 1; y++)
            {
                const int xy = CoordsToIndex(flipDirection ? x + 1 : x, flipDirection ? y + 1 : y, vertexSideCount);
                const int x1y1 = CoordsToIndex(flipDirection ? x : x + 1, flipDirection ? y : y + 1, vertexSideCount);

                outIndices.Append(indexOffset + xy);
                outIndices.Append(indexOffset + CoordsToIndex(x, y + 1, vertexSideCount));
                outIndices.Append(indexOffset + x1y1);

                outIndices.Append(indexOffset + x1y1);
                outIndices.Append(indexOffset + CoordsToIndex(x + 1, y, vertexSideCount));
                outIndices.Append(indexOffset + xy);
            }
        }
    }

    void MeshUtils::CreateXYGrid(const Vector2& size, const Vector3& offset, Mesh& mesh, VertexChannelFlags channels,
        uint32 subdivisions, bool flipDirection)
    {
        Array<Vector3> positions;
        Array<uint32> indices;
        Array<Vector3> normals;
        Array<Vector2> uvs;

        bool generateTangents = (channels & VertexChannelFlags::Tangent) == VertexChannelFlags::Tangent;

        if (generateTangents)
            channels |= VertexChannelFlags::Normal | VertexChannelFlags::UV0;

        bool useNormals = (channels & VertexChannelFlags::Normal) == VertexChannelFlags::Normal;
        bool useUvs = (channels & VertexChannelFlags::UV0) == VertexChannelFlags::UV0;

        CreateXYGrid(
            size,
            offset,
            positions,
            indices,
            useNormals ? &normals : nullptr,
            useUvs ? &uvs : nullptr,
            subdivisions,
            flipDirection);

        mesh.SetPositions(positions);

        if (useNormals)
            mesh.SetNormals(normals);

        if (useUvs)
            mesh.SetUVs(uvs);

        //Array<IndexData, 1> meshIndexData = { IndexData(indices) };
        //mesh.SetIndicies(meshIndexData);
        mesh.SetIndices(indices);

        if (generateTangents)
            mesh.CalculateTangents();
    }

    void MeshUtils::CreateXZGrid(const Vector2& size, const Vector3& offset,
        ArrayContainer<Vector3>& outVertexPositions, ArrayContainer<uint32>& outIndices,
        ArrayContainer<Vector3>* outVertexNormals, ArrayContainer<Vector2>* outVertexUVs, uint32 subdivisions,
        bool flipDirection)
    {
        const uint32 vertexSideCount = 2 + subdivisions;
        const float vertexScaling = 1.0f / (vertexSideCount - 1);

        uint32 totalVertexCount = vertexSideCount * vertexSideCount;
        outVertexPositions.Reserve(outVertexPositions.GetCount() + totalVertexCount);

        if (outVertexNormals)
            outVertexNormals->Reserve(outVertexNormals->GetCount() + totalVertexCount);

        if (outVertexUVs)
            outVertexUVs->Reserve(outVertexUVs->GetCount() + totalVertexCount);

        const uint32 indexOffset = static_cast<uint32>(outVertexPositions.GetCount());

        Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

        for (uint32 x = 0; x < vertexSideCount; x++)
        {
            for (uint32 z = 0; z < vertexSideCount; z++)
            {
                const float u = static_cast<float>(x) / (vertexSideCount - 1);
                const float v = 1.0f - (static_cast<float>(z) / (vertexSideCount - 1));

                outVertexPositions.EmplaceBack((x * vertexScaling - 0.5f) * size.X() + offset.X(), offset.Y(), (z * vertexScaling - 0.5f) * size.Y() + offset.Z());

                if (outVertexNormals)
                    outVertexNormals->Append(normalDir);

                if (outVertexUVs)
                    outVertexUVs->EmplaceBack(flipDirection ? 1.0f - u : u, v);
            }
        }

        outIndices.Reserve(outIndices.GetCount() + (vertexSideCount - 1) * (vertexSideCount - 1) * 6);

        for (uint32 x = 0; x < vertexSideCount - 1; x++)
        {
            for (uint32 z = 0; z < vertexSideCount - 1; z++)
            {
                const int xz = CoordsToIndex(flipDirection ? x + 1 : x, flipDirection ? z + 1 : z, vertexSideCount);
                const int x1z1 = CoordsToIndex(flipDirection ? x : x + 1, flipDirection ? z : z + 1, vertexSideCount);

                outIndices.Append(indexOffset + xz);
                outIndices.Append(indexOffset + CoordsToIndex(x + 1, z, vertexSideCount));
                outIndices.Append(indexOffset + x1z1);

                outIndices.Append(indexOffset + x1z1);
                outIndices.Append(indexOffset + CoordsToIndex(x, z + 1, vertexSideCount));
                outIndices.Append(indexOffset + xz);
            }
        }
    }

    void MeshUtils::CreateXZGrid(const Vector2& size, const Vector3& offset, Mesh& mesh, VertexChannelFlags channels,
        uint32 subdivisions, bool flipDirection)
    {
        Array<Vector3> positions;
        Array<uint32> indices;
        Array<Vector3> normals;
        Array<Vector2> uvs;

        bool generateTangents = (channels & VertexChannelFlags::Tangent) == VertexChannelFlags::Tangent;

        if (generateTangents)
            channels |= VertexChannelFlags::Normal | VertexChannelFlags::UV0;

        bool useNormals = (channels & VertexChannelFlags::Normal) == VertexChannelFlags::Normal;
        bool useUvs = (channels & VertexChannelFlags::UV0) == VertexChannelFlags::UV0;

        CreateXZGrid(
            size,
            offset,
            positions,
            indices,
            useNormals ? &normals : nullptr,
            useUvs ? &uvs : nullptr,
            subdivisions,
            flipDirection);

        mesh.SetPositions(positions);

        if (useNormals)
            mesh.SetNormals(normals);

        if (useUvs)
            mesh.SetUVs(uvs);

        //Array<IndexData, 1> meshIndexData = { IndexData(indices) };
        //mesh.SetIndicies(meshIndexData);
        mesh.SetIndices(indices);

        if (generateTangents)
            mesh.CalculateTangents();
    }

    void MeshUtils::CreateZYGrid(const Vector2& size, const Vector3& offset,
        ArrayContainer<Vector3>& outVertexPositions, ArrayContainer<uint32>& outIndices,
        ArrayContainer<Vector3>* outVertexNormals, ArrayContainer<Vector2>* outVertexUVs, uint32 subdivisions,
        bool flipDirection)
    {
        const uint32 vertexSideCount = 2 + subdivisions;
        const float vertexScaling = 1.0f / (vertexSideCount - 1);

        uint32 totalVertexCount = vertexSideCount * vertexSideCount;
        outVertexPositions.Reserve(outVertexPositions.GetCount() + totalVertexCount);

        if (outVertexNormals)
            outVertexNormals->Reserve(outVertexNormals->GetCount() + totalVertexCount);

        if (outVertexUVs)
            outVertexUVs->Reserve(outVertexUVs->GetCount() + totalVertexCount);

        const uint32 indexOffset = static_cast<uint32>(outVertexPositions.GetCount());

        Vector3 normalDir = flipDirection ? Vector3::Left : Vector3::Right;

        for (uint32 z = 0; z < vertexSideCount; z++)
        {
            for (uint32 y = 0; y < vertexSideCount; y++)
            {
                const float u = 1.0f - (static_cast<float>(z) / (vertexSideCount - 1));
                const float v = static_cast<float>(y) / (vertexSideCount - 1);

                outVertexPositions.EmplaceBack(offset.X(), (y * vertexScaling - 0.5f) * size.Y() + offset.Y(), (z * vertexScaling - 0.5f) * size.X() + offset.Z());

                if (outVertexNormals)
                    outVertexNormals->Append(normalDir);

                if (outVertexUVs)
                    outVertexUVs->EmplaceBack(flipDirection ? 1.0f - u : u, v);
            }
        }

        outIndices.Reserve(outIndices.GetCount() + (vertexSideCount - 1) * (vertexSideCount - 1) * 6);

        for (uint32 z = 0; z < vertexSideCount - 1; z++)
        {
            for (uint32 y = 0; y < vertexSideCount - 1; y++)
            {
                const int zy = CoordsToIndex(flipDirection ? z + 1 : z, flipDirection ? y + 1 : y, vertexSideCount);
                const int z1y1 = CoordsToIndex(flipDirection ? z : z + 1, flipDirection ? y : y + 1, vertexSideCount);

                outIndices.Append(indexOffset + zy);
                outIndices.Append(indexOffset + CoordsToIndex(z + 1, y, vertexSideCount));
                outIndices.Append(indexOffset + z1y1);

                outIndices.Append(indexOffset + z1y1);
                outIndices.Append(indexOffset + CoordsToIndex(z, y + 1, vertexSideCount));
                outIndices.Append(indexOffset + zy);
            }
        }
    }

    void MeshUtils::CreateZYGrid(const Vector2& size, const Vector3& offset, Mesh& mesh, VertexChannelFlags channels,
        uint32 subdivisions, bool flipDirection)
    {
        Array<Vector3> positions;
        Array<uint32> indices;
        Array<Vector3> normals;
        Array<Vector2> uvs;

        bool generateTangents = (channels & VertexChannelFlags::Tangent) == VertexChannelFlags::Tangent;

        if (generateTangents)
            channels |= VertexChannelFlags::Normal | VertexChannelFlags::UV0;

        bool useNormals = (channels & VertexChannelFlags::Normal) == VertexChannelFlags::Normal;
        bool useUvs = (channels & VertexChannelFlags::UV0) == VertexChannelFlags::UV0;

        CreateZYGrid(
            size,
            offset,
            positions,
            indices,
            useNormals ? &normals : nullptr,
            useUvs ? &uvs : nullptr,
            subdivisions,
            flipDirection);

        mesh.SetPositions(positions);

        if (useNormals)
            mesh.SetNormals(normals);

        if (useUvs)
            mesh.SetUVs(uvs);

        //Array<IndexData, 1> meshIndexData = { IndexData(indices) };
        //mesh.SetIndicies(meshIndexData);
        mesh.SetIndices(indices);

        if (generateTangents)
            mesh.CalculateTangents();
    }

    void MeshUtils::CreateCube(const Vector3& size, const Vector3& offset, ArrayContainer<Vector3>& outVertexPositions,
        ArrayContainer<uint32>& outIndices, ArrayContainer<Vector3>* outVertexNormals,
        ArrayContainer<Vector2>* outVertexUVs, uint32 subdivisions, bool flipDirection)
    {
        Vector3 sizeOffset = size * 0.5f;

        // X face
        CreateZYGrid(
            Vector2(size.Z(), size.Y()), 
            Vector3::Right * sizeOffset.X() + offset, 
            outVertexPositions, 
            outIndices, 
            outVertexNormals, 
            outVertexUVs, 
            subdivisions, 
            flipDirection);

        // -X face
        CreateZYGrid(
            Vector2(size.Z(), size.Y()),
            Vector3::Left * sizeOffset.X() + offset,
            outVertexPositions,
            outIndices,
            outVertexNormals,
            outVertexUVs,
            subdivisions,
            !flipDirection);

        // Y face
        CreateXZGrid(
            Vector2(size.X(), size.Z()),
            Vector3::Up * sizeOffset.Y() + offset,
            outVertexPositions,
            outIndices,
            outVertexNormals,
            outVertexUVs,
            subdivisions,
            flipDirection);

        // -Y face
        CreateXZGrid(
            Vector2(size.X(), size.Z()),
            Vector3::Down * sizeOffset.Y() + offset,
            outVertexPositions,
            outIndices,
            outVertexNormals,
            outVertexUVs,
            subdivisions,
            !flipDirection);

        // Z face
        CreateXYGrid(
            Vector2(size.X(), size.Y()),
            Vector3::Backward * sizeOffset.Z() + offset,
            outVertexPositions,
            outIndices,
            outVertexNormals,
            outVertexUVs,
            subdivisions,
            flipDirection);

        // -Z face
        CreateXYGrid(
            Vector2(size.X(), size.Y()),
            Vector3::Forward * sizeOffset.Z() + offset,
            outVertexPositions,
            outIndices,
            outVertexNormals,
            outVertexUVs,
            subdivisions,
            !flipDirection);
    }

    void MeshUtils::CreateCube(const Vector3& size, const Vector3& offset, Mesh& mesh, VertexChannelFlags channels,
        uint32 subdivisions, bool flipDirection)
    {
        Array<Vector3> positions;
        Array<uint32> indices;
        Array<Vector3> normals;
        Array<Vector2> uvs;

        bool generateTangents = (channels & VertexChannelFlags::Tangent) == VertexChannelFlags::Tangent;

        if (generateTangents)
            channels |= VertexChannelFlags::Normal | VertexChannelFlags::UV0;

        bool useNormals = (channels & VertexChannelFlags::Normal) == VertexChannelFlags::Normal;
        bool useUvs = (channels & VertexChannelFlags::UV0) == VertexChannelFlags::UV0;

        CreateCube(
            size,
            offset,
            positions,
            indices,
            useNormals ? &normals : nullptr,
            useUvs ? &uvs : nullptr,
            subdivisions,
            flipDirection);

        mesh.SetPositions(positions);

        if (useNormals)
            mesh.SetNormals(normals);

        if (useUvs)
            mesh.SetUVs(uvs);

        //Array<IndexData, 1> meshIndexData = { IndexData(indices) };
        //mesh.SetIndicies(meshIndexData);
        mesh.SetIndices(indices);

        if (generateTangents)
            mesh.CalculateTangents();
    }

    void MeshUtils::CreateXYEllipse(const Vector2& radii, const Vector3& offset, uint32 vertices,
        ArrayContainer<Vector3>& outVertexPositions, ArrayContainer<uint32>& outIndices,
        ArrayContainer<Vector3>* outVertexNormals, ArrayContainer<Vector2>* outVertexUVs, bool flipDirection)
    {
        if (vertices < 3)
            return;

        outVertexPositions.Reserve(outVertexPositions.GetCount() + vertices + 1);

        const uint32 indexOffset = static_cast<uint32>(outVertexPositions.GetCount());

        Vector3 normalDir = flipDirection ? Vector3::Forward : Vector3::Backward;

        for (uint32 i = 0; i < vertices; i++)
        {
            // Draw vertices clockwise
            float a = -(static_cast<float>(i) / vertices);
            float c = Math::Cos(a * Math::TwoPI);
            float s = Math::Sin(a * Math::TwoPI);

            outVertexPositions.EmplaceBack(c * radii.X() + offset.X(), s * radii.Y() + offset.Y(), offset.Z());

            if (outVertexNormals)
                outVertexNormals->EmplaceBack(normalDir);

            if (outVertexUVs)
            {
                float u = c * 0.5f + 0.5f;
                outVertexUVs->EmplaceBack(flipDirection ? 1.0f - u : u, s * 0.5f + 0.5f);
            }
        }

        // Create an anchor for the triangle fan
        outVertexPositions.EmplaceBack(offset);

        if (outVertexNormals)
            outVertexNormals->EmplaceBack(normalDir);

        if (outVertexUVs)
            outVertexUVs->EmplaceBack(0.5f, 0.5f);

        outIndices.Reserve(outIndices.GetCount() + (vertices - 2));

        for (uint32 i = 0; i < vertices; i++)
        {
            uint32 i2 = (i + 1) % vertices;
            outIndices.Append(indexOffset + (flipDirection ? i2 : i));
            outIndices.Append(indexOffset + (flipDirection ? i : i2));
            outIndices.Append(indexOffset + vertices);
        }

        #if 0
        uint32 increment = 1;
        uint32 currentIndex = 0;
        uint32 iterations = std::floor(Math::Sqrt(vertices - 2));

        for (uint32 i = 0; i < iterations; i++)
        {
            for (uint32 c = 0; c * increment < vertices; c += increment * 2)
            {
                outIndices.Append(indexOffset + currentIndex);
                currentIndex += increment;

                if (currentIndex >= vertices)
                {
                    currentIndex -= vertices;
                    increment *= 2;
                }

                outIndices.Append(indexOffset + currentIndex);
                currentIndex += increment;

                if (currentIndex >= vertices)
                {
                    currentIndex -= vertices;
                    increment *= 2;
                }

                outIndices.Append(indexOffset + currentIndex);
            }
        }
        #endif
    }

    void MeshUtils::CreateXYEllipse(const Vector2& radii, const Vector3& offset, uint32 vertices, Mesh& mesh,
        VertexChannelFlags channels, bool flipDirection)
    {
        Array<Vector3> positions;
        Array<uint32> indices;
        Array<Vector3> normals;
        Array<Vector2> uvs;

        bool generateTangents = (channels & VertexChannelFlags::Tangent) == VertexChannelFlags::Tangent;

        if (generateTangents)
            channels |= VertexChannelFlags::Normal | VertexChannelFlags::UV0;

        bool useNormals = (channels & VertexChannelFlags::Normal) == VertexChannelFlags::Normal;
        bool useUvs = (channels & VertexChannelFlags::UV0) == VertexChannelFlags::UV0;

        CreateXYEllipse(
            radii,
            offset,
            vertices,
            positions,
            indices,
            useNormals ? &normals : nullptr,
            useUvs ? &uvs : nullptr,
            flipDirection);

        mesh.SetPositions(positions);

        if (useNormals)
            mesh.SetNormals(normals);

        if (useUvs)
            mesh.SetUVs(uvs);

        //Array<IndexData, 1> meshIndexData = { IndexData(indices) };
        //mesh.SetIndicies(meshIndexData);
        mesh.SetIndices(indices);

        if (generateTangents)
            mesh.CalculateTangents();
    }

    void MeshUtils::CreateXZEllipse(const Vector2& radii, const Vector3& offset, uint32 vertices,
        ArrayContainer<Vector3>& outVertexPositions, ArrayContainer<uint32>& outIndices,
        ArrayContainer<Vector3>* outVertexNormals, ArrayContainer<Vector2>* outVertexUVs, bool flipDirection)
    {
        if (vertices < 3)
            return;

        outVertexPositions.Reserve(outVertexPositions.GetCount() + vertices + 1);

        const uint32 indexOffset = static_cast<uint32>(outVertexPositions.GetCount());

        Vector3 normalDir = flipDirection ? Vector3::Down : Vector3::Up;

        for (uint32 i = 0; i < vertices; i++)
        {
            // Draw vertices clockwise
            float a = -(static_cast<float>(i) / vertices);
            float c = Math::Cos(a * Math::TwoPI);
            float s = -Math::Sin(a * Math::TwoPI);

            outVertexPositions.EmplaceBack(c * radii.X() + offset.X(), offset.Y(), s * radii.Y() + offset.Z());

            if (outVertexNormals)
                outVertexNormals->EmplaceBack(normalDir);

            if (outVertexUVs)
            {
                float u = c * 0.5f + 0.5f;
                outVertexUVs->EmplaceBack(flipDirection ? 1.0f - u : u, -s * 0.5f + 0.5f);
            }
        }

        // Create an anchor for the triangle fan
        outVertexPositions.EmplaceBack(offset);

        if (outVertexNormals)
            outVertexNormals->EmplaceBack(normalDir);

        if (outVertexUVs)
            outVertexUVs->EmplaceBack(0.5f, 0.5f);

        outIndices.Reserve(outIndices.GetCount() + (vertices - 2));

        for (uint32 i = 0; i < vertices; i++)
        {
            uint32 i2 = (i + 1) % vertices;
            outIndices.Append(indexOffset + (flipDirection ? i2 : i));
            outIndices.Append(indexOffset + (flipDirection ? i : i2));
            outIndices.Append(indexOffset + vertices);
        }
    }

    void MeshUtils::CreateXZEllipse(const Vector2& radii, const Vector3& offset, uint32 vertices, Mesh& mesh,
        VertexChannelFlags channels, bool flipDirection)
    {
        Array<Vector3> positions;
        Array<uint32> indices;
        Array<Vector3> normals;
        Array<Vector2> uvs;

        bool generateTangents = (channels & VertexChannelFlags::Tangent) == VertexChannelFlags::Tangent;

        if (generateTangents)
            channels |= VertexChannelFlags::Normal | VertexChannelFlags::UV0;

        bool useNormals = (channels & VertexChannelFlags::Normal) == VertexChannelFlags::Normal;
        bool useUvs = (channels & VertexChannelFlags::UV0) == VertexChannelFlags::UV0;

        CreateXZEllipse(
            radii,
            offset,
            vertices,
            positions,
            indices,
            useNormals ? &normals : nullptr,
            useUvs ? &uvs : nullptr,
            flipDirection);

        mesh.SetPositions(positions);

        if (useNormals)
            mesh.SetNormals(normals);

        if (useUvs)
            mesh.SetUVs(uvs);

        //Array<IndexData, 1> meshIndexData = { IndexData(indices) };
        //mesh.SetIndicies(meshIndexData);
        mesh.SetIndices(indices);

        if (generateTangents)
            mesh.CalculateTangents();
    }

    void MeshUtils::CreateZYEllipse(const Vector2& radii, const Vector3& offset, uint32 vertices,
        ArrayContainer<Vector3>& outVertexPositions, ArrayContainer<uint32>& outIndices,
        ArrayContainer<Vector3>* outVertexNormals, ArrayContainer<Vector2>* outVertexUVs, bool flipDirection)
    {
        if (vertices < 3)
            return;

        outVertexPositions.Reserve(outVertexPositions.GetCount() + vertices + 1);

        const uint32 indexOffset = static_cast<uint32>(outVertexPositions.GetCount());

        Vector3 normalDir = flipDirection ? Vector3::Left : Vector3::Right;

        for (uint32 i = 0; i < vertices; i++)
        {
            // Draw vertices clockwise
            float a = -(static_cast<float>(i) / vertices);
            float c = -Math::Cos(a * Math::TwoPI);
            float s = Math::Sin(a * Math::TwoPI);

            outVertexPositions.EmplaceBack(offset.X(), s * radii.Y() + offset.Y(), c * radii.X() + offset.Z());

            if (outVertexNormals)
                outVertexNormals->EmplaceBack(normalDir);

            if (outVertexUVs)
            {
                float u = c * 0.5f + 0.5f;
                outVertexUVs->EmplaceBack(flipDirection ? u : 1.0f - u, s * 0.5f + 0.5f);
            }
        }

        // Create an anchor for the triangle fan
        outVertexPositions.EmplaceBack(offset);

        if (outVertexNormals)
            outVertexNormals->EmplaceBack(normalDir);

        if (outVertexUVs)
            outVertexUVs->EmplaceBack(0.5f, 0.5f);

        outIndices.Reserve(outIndices.GetCount() + (vertices - 2));

        for (uint32 i = 0; i < vertices; i++)
        {
            uint32 i2 = (i + 1) % vertices;
            outIndices.Append(indexOffset + (flipDirection ? i2 : i));
            outIndices.Append(indexOffset + (flipDirection ? i : i2));
            outIndices.Append(indexOffset + vertices);
        }
    }

    void MeshUtils::CreateZYEllipse(const Vector2& radii, const Vector3& offset, uint32 vertices, Mesh& mesh,
        VertexChannelFlags channels, bool flipDirection)
    {
        Array<Vector3> positions;
        Array<uint32> indices;
        Array<Vector3> normals;
        Array<Vector2> uvs;

        bool generateTangents = (channels & VertexChannelFlags::Tangent) == VertexChannelFlags::Tangent;

        if (generateTangents)
            channels |= VertexChannelFlags::Normal | VertexChannelFlags::UV0;

        bool useNormals = (channels & VertexChannelFlags::Normal) == VertexChannelFlags::Normal;
        bool useUvs = (channels & VertexChannelFlags::UV0) == VertexChannelFlags::UV0;

        CreateZYEllipse(
            radii,
            offset,
            vertices,
            positions,
            indices,
            useNormals ? &normals : nullptr,
            useUvs ? &uvs : nullptr,
            flipDirection);

        mesh.SetPositions(positions);

        if (useNormals)
            mesh.SetNormals(normals);

        if (useUvs)
            mesh.SetUVs(uvs);

        //Array<IndexData, 1> meshIndexData = { IndexData(indices) };
        //mesh.SetIndicies(meshIndexData);
        mesh.SetIndices(indices);

        if (generateTangents)
            mesh.CalculateTangents();
    }

    uint32 MeshUtils::CoordsToIndex(uint32 x, uint32 y, uint32 length)
    {
        return x * length + y;
    }
} // Coco