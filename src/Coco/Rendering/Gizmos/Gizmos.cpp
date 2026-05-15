//
// Created by cullen on 3/31/26.
//

#include "Gizmos.h"

#include "GizmosRenderPass.h"

#include "Coco/Core/Engine.h"
#include "Coco/Rendering/RenderService.h"

namespace Coco
{
    GizmoDrawCall::GizmoDrawCall(uint8 submeshID, const Vector4& color, const Matrix4x4& transform) :
        SubmeshID(submeshID),
        DrawColor(color),
        Transform(transform)
    {}

    Gizmos::Gizmos(RenderService* renderService) :
        _mesh(),
        _lineSubmesh(0),
        _boxSubmesh(0),
        _circleSubmesh(0),
        _sphereSubmesh(0),
        _coneSubmesh(0),
        _drawCalls(nullptr, 100),
        _clearTickListener(this, &Gizmos::OnClearTick, ClearTickOrder)
    {
        CreateResources();
        _clearTickListener.ListenTo(*renderService->GetEngine()->GetMainLoop());
    }

    Gizmos::~Gizmos()
    {
        _clearTickListener.StopListening();
        _drawCalls.Clear(true);
        Engine::Get()->GetResourceManager()->RemoveResource(_mesh->GetID());
    }

    void Gizmos::Clear()
    {
        _drawCalls.Clear();
    }

    void Gizmos::DrawLine3D(const Vector3& start, const Vector3& end, const Color& color)
    {
        Vector3 dir(end - start);
        DrawRay3D(start, dir, color);
    }

    void Gizmos::DrawRay3D(const Vector3& origin, const Vector3& direction, const Color& color)
    {
        Matrix4x4 t = Matrix4x4::CreateTransform(origin, Quaternion::FromToRotation(Vector3::Forward, direction.Normalized()), Vector3::One * direction.GetLength<float>());
        _drawCalls.EmplaceBack(_lineSubmesh, color.AsVector4(false), t);
    }

    void Gizmos::DrawBox3D(const Vector3& origin, const Quaternion& rotation, const Vector3& size, const Color& color)
    {
        Matrix4x4 t = Matrix4x4::CreateTransform(origin, rotation, size);
        _drawCalls.EmplaceBack(_boxSubmesh, color.AsVector4(false), t);
    }

    void Gizmos::DrawCircle3D(const Vector3& origin, const Quaternion& rotation, float radius, const Color& color)
    {
        Matrix4x4 t = Matrix4x4::CreateTransform(origin, rotation, Vector3::One * radius);
        _drawCalls.EmplaceBack(_circleSubmesh, color.AsVector4(false), t);
    }

    void Gizmos::DrawSphere3D(const Vector3& origin, float radius, const Color& color)
    {
        Matrix4x4 t = Matrix4x4::CreateTransform(origin, Quaternion::Identity, Vector3::One * radius);
        _drawCalls.EmplaceBack(_sphereSubmesh, color.AsVector4(false), t);
    }

    void Gizmos::DrawCone3D(const Vector3& origin, const Quaternion& rotation, float height, float radius,
        const Color& color)
    {
        Matrix4x4 t = Matrix4x4::CreateTransform(origin, rotation, Vector3(radius, height, radius));
        _drawCalls.EmplaceBack(_coneSubmesh, color.AsVector4(false), t);
    }

    void Gizmos::Render(RenderGraph& graph, RenderScene& scene)
    {
        uint64 id = typeid(Gizmos).hash_code();

        for (uint64 i = 0; i < _drawCalls.GetCount(); i++)
        {
            uint64 objID = Math::CombineHashes(id, i);

            const auto& dc = _drawCalls[i];
            GizmoObjectData obj{dc.Transform, dc.DrawColor};
            scene.StoreData(objID, true, obj);

            scene.AddObject(objID, 0, 0.0f, *_mesh, dc.SubmeshID);
        }

        graph.CreateRenderPassObject<GizmosRenderPass>("Gizmos");
    }

    void Gizmos::CreateLineMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices)
    {
        uint32 vertexOffset = static_cast<uint32>(outVerts.GetCount());

        // Line points forward (0, 0, 0) -> (0, 0, -1)
        outVerts.EmplaceBack(Vector3::Zero);
        outVerts.EmplaceBack(Vector3::Forward);

        outIndices.Append(vertexOffset);
        outIndices.Append(vertexOffset + 1);
    }

    void Gizmos::CreateBoxMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices)
    {
        uint32 vertexOffset = static_cast<uint32>(outVerts.GetCount());

        //   2-------3
        //  /|      /|
        // 6-------7 |
        // | |     | |
        // | 0-----|-1
        // |/      |/
        // 4-------5

        outVerts.EmplaceBack(Vector3(-0.5f, -0.5f, -0.5f));
        outVerts.EmplaceBack(Vector3(0.5f, -0.5f, -0.5f));
        outVerts.EmplaceBack(Vector3(-0.5f, 0.5f, -0.5f));
        outVerts.EmplaceBack(Vector3(0.5f, 0.5f, -0.5f));

        outVerts.EmplaceBack(Vector3(-0.5f, -0.5f, 0.5f));
        outVerts.EmplaceBack(Vector3(0.5f, -0.5f, 0.5f));
        outVerts.EmplaceBack(Vector3(-0.5f, 0.5f, 0.5f));
        outVerts.EmplaceBack(Vector3(0.5f, 0.5f, 0.5f));

        outIndices.Append(vertexOffset);
        outIndices.Append(vertexOffset + 1);

        outIndices.Append(vertexOffset);
        outIndices.Append(vertexOffset + 2);

        outIndices.Append(vertexOffset);
        outIndices.Append(vertexOffset + 4);

        outIndices.Append(vertexOffset + 3);
        outIndices.Append(vertexOffset + 1);

        outIndices.Append(vertexOffset + 3);
        outIndices.Append(vertexOffset + 2);

        outIndices.Append(vertexOffset + 3);
        outIndices.Append(vertexOffset + 7);

        outIndices.Append(vertexOffset + 5);
        outIndices.Append(vertexOffset + 4);

        outIndices.Append(vertexOffset + 5);
        outIndices.Append(vertexOffset + 7);

        outIndices.Append(vertexOffset + 5);
        outIndices.Append(vertexOffset + 1);

        outIndices.Append(vertexOffset + 6);
        outIndices.Append(vertexOffset + 4);

        outIndices.Append(vertexOffset + 6);
        outIndices.Append(vertexOffset + 7);

        outIndices.Append(vertexOffset + 6);
        outIndices.Append(vertexOffset + 2);
    }

    void Gizmos::CreateCircleMesh(const Quaternion& rotation, ArrayContainer<Vector3>& outVerts,
        ArrayContainer<uint32>& outIndices)
    {
        uint32 vertexOffset = static_cast<uint32>(outVerts.GetCount());

        const float radius = 1.0f;
        const uint32 resolution = 32;

        // XY circle
        for (uint32 z = 0; z < resolution; z++)
        {
            float r = (static_cast<float>(z) / resolution) * Math::TwoPI;
            outVerts.EmplaceBack(rotation * Vector3(Math::Cos(r), Math::Sin(r), 0.0f) * radius);
        }

        for (uint32 i = 0; i < resolution; i++)
        {
            uint32 i0 = vertexOffset + i;
            uint32 i1 = vertexOffset + ((i + 1) % resolution);

            outIndices.Append(i0);
            outIndices.Append(i1);
        }
    }

    void Gizmos::CreateSphereMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices)
    {
        CreateCircleMesh(Quaternion::Identity, outVerts, outIndices);
        CreateCircleMesh(Quaternion(Vector3::Up, Math::DegToRad(90.0f)), outVerts, outIndices);
        CreateCircleMesh(Quaternion(Vector3::Right, Math::DegToRad(90.0f)), outVerts, outIndices);
    }

    void Gizmos::CreateConeMesh(ArrayContainer<Vector3>& outVerts, ArrayContainer<uint32>& outIndices)
    {
        // Circle is at base
        CreateCircleMesh(Quaternion(Vector3::Right, Math::DegToRad(90.0f)), outVerts, outIndices);

        uint32 vertexOffset = static_cast<uint32>(outVerts.GetCount());

        const float height = 1.0f;

        // +X line
        outVerts.EmplaceBack(Vector3(1.0f, 0.0f, 0.0f));
        outVerts.EmplaceBack(Vector3(0.0f, height, 0.0f));

        outIndices.Append(vertexOffset);
        outIndices.Append(vertexOffset + 1);

        // -X line
        outVerts.EmplaceBack(Vector3(-1.0f, 0.0f, 0.0f));
        outVerts.EmplaceBack(Vector3(0.0f, height, 0.0f));

        outIndices.Append(vertexOffset + 2);
        outIndices.Append(vertexOffset + 3);

        // +Z line
        outVerts.EmplaceBack(Vector3(0.0f, 0.0f, 1.0f));
        outVerts.EmplaceBack(Vector3(0.0f, height, 0.0f));

        outIndices.Append(vertexOffset + 4);
        outIndices.Append(vertexOffset + 5);

        // -Z line
        outVerts.EmplaceBack(Vector3(0.0f, 0.0f, -1.0f));
        outVerts.EmplaceBack(Vector3(0.0f, height, 0.0f));

        outIndices.Append(vertexOffset + 6);
        outIndices.Append(vertexOffset + 7);
    }

    void Gizmos::CreateResources()
    {
        Engine* engine = Engine::Get();
        Array<Vector3> vertices(engine->GetTemporaryStackAllocator(), 210);
        Array<uint32> indices(engine->GetTemporaryStackAllocator(), 420);
        Array<Submesh> submeshes(engine->GetTemporaryStackAllocator(), 5);

        CreateLineMesh(vertices, indices);
        _lineSubmesh = submeshes.GetCount();
        submeshes.EmplaceBack(0, indices.GetCount());

        uint64 offset = indices.GetCount();
        CreateBoxMesh(vertices, indices);
        _boxSubmesh = submeshes.GetCount();
        submeshes.EmplaceBack(offset, indices.GetCount() - offset);

        offset = indices.GetCount();
        CreateCircleMesh(Quaternion::Identity, vertices, indices);
        _circleSubmesh = submeshes.GetCount();
        submeshes.EmplaceBack(offset, indices.GetCount() - offset);

        offset = indices.GetCount();
        CreateSphereMesh(vertices, indices);
        _sphereSubmesh = submeshes.GetCount();
        submeshes.EmplaceBack(offset, indices.GetCount() - offset);

        offset = indices.GetCount();
        CreateConeMesh(vertices, indices);
        _coneSubmesh = submeshes.GetCount();
        submeshes.EmplaceBack(offset, indices.GetCount() - offset);

        _mesh = engine->GetResourceManager()->CreateResource<Mesh>("Gizmos", false);
        _mesh->SetPositions(vertices);
        _mesh->SetIndices(indices);
        _mesh->SetSubmeshes(submeshes);
    }

    void Gizmos::OnClearTick(const TickInfo& tickInfo)
    {
        Clear();
    }
} // Coco