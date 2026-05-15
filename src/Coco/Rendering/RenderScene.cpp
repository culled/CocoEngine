//
// Created by cullen on 3/7/26.
//

#include "RenderScene.h"

#include "Mesh.h"
#include "Shader.h"
#include "Graphics/RenderFrame.h"
#include "Graphics/ShaderUniformValue.h"
#include <Coco/Core/Engine.h>

#include "Material.h"

namespace Coco
{
    RenderScene::RenderScene(RenderFrame* frame, uint64 id, const Sizei& frameSize) :
        _frame(frame),
        _id(id),
        _frameSize(frameSize),
        _viewPosition(),
        _viewRotation(),
        _firstRenderObjectIndex(frame->_renderObjects.GetCount()),
        _renderObjectCount(0)
    {}

    Matrix4x4 RenderScene::CreateOrthographicProjection(float size, float nearClip, float farClip) const
    {
        float aspect = static_cast<float>(_frameSize.Width) / static_cast<float>(_frameSize.Height);
        float halfSize = size * 0.5f;
        return CreateOrthographicProjection(-halfSize * aspect, halfSize * aspect, -halfSize, halfSize, nearClip, farClip);
    }

    Matrix4x4 RenderScene::CreateOrthographicProjection(float left, float right, float bottom, float top,
        float nearClip, float farClip) const
    {
        return _frame->CreateOrthographicProjection(left, right, bottom, top, nearClip, farClip);
    }

    Matrix4x4 RenderScene::CreatePerspectiveProjection(float verticalFOV, float nearClip, float farClip) const
    {
        float aspect = static_cast<float>(_frameSize.Width) / static_cast<float>(_frameSize.Height);
        return CreatePerspectiveProjection(verticalFOV, aspect, nearClip, farClip);
    }

    Matrix4x4 RenderScene::CreatePerspectiveProjection(float verticalFOV, float aspectRatio, float nearClip,
        float farClip) const
    {
        return _frame->CreatePerspectiveProjection(verticalFOV, aspectRatio, nearClip, farClip);
    }

    void RenderScene::SetOrthographicCamera(const Vector3& position, const Quaternion& rotation, float size,
                                            float nearClip, float farClip)
    {
        _viewPosition = position;
        _viewRotation = rotation;

        _viewMatrix = Matrix4x4::CreateView(position, rotation);
        _projectionMatrix = CreateOrthographicProjection(size, nearClip, farClip);
    }

    void RenderScene::SetOrthographicCamera(const Vector3& position, const Vector3& target, const Vector3& up,
        float size, float nearClip, float farClip)
    {
        _viewPosition = position;

        _viewMatrix = Matrix4x4::CreateLookAt(position, target, up);
        _viewRotation = _viewMatrix.Inverse().GetRotation();
        _projectionMatrix = CreateOrthographicProjection(size, nearClip, farClip);
    }

    void RenderScene::SetOrthographicCamera(const Vector2& position, float rotation, float size, float nearClip,
        float farClip)
    {
        SetOrthographicCamera(Vector3(position, 0.0f), Quaternion(Vector3(0.0f, 0.0f, rotation)), size, nearClip, farClip);
    }

    void RenderScene::SetPerspectiveCamera(const Vector3& position, const Quaternion& rotation, float verticalFOV,
                                           float nearClip, float farClip)
    {
        _viewPosition = position;
        _viewRotation = rotation;

        _viewMatrix = Matrix4x4::CreateView(position, rotation);
        _projectionMatrix = CreatePerspectiveProjection(verticalFOV, nearClip, farClip);
    }

    void RenderScene::SetPerspectiveCamera(const Vector3& position, const Vector3& target, const Vector3& up,
                                           float verticalFOV, float nearClip, float farClip)
    {
        _viewPosition = position;

        _viewMatrix = Matrix4x4::CreateLookAt(position, target, up);
        _viewRotation = _viewMatrix.Inverse().GetRotation();
        _projectionMatrix = CreatePerspectiveProjection(verticalFOV, nearClip, farClip);
    }

    SharedPtr<Texture> RenderScene::GetTexture(uint64 id) const
    {
        if (id == 0)
            return nullptr;

        return Engine::Get()->GetResourceManager()->GetResourceAs<Texture>(id);
    }

    void RenderScene::StoreUniforms(uint64 id, bool sharedData, Span<const ShaderUniformValue> uniforms)
    {
        uint64 dataID = GetDataID(id, sharedData);
        _frame->_renderSceneStorage.StoreUniforms(dataID, uniforms);
    }

    bool RenderScene::HasUniforms(uint64 id, bool sharedData) const
    {
        uint64 dataID = GetDataID(id, sharedData);
        return _frame->_renderSceneStorage.HasUniforms(dataID);
    }

    Span<const ShaderUniformValue> RenderScene::GetUniforms(uint64 id, bool sharedData) const
    {
        uint64 dataID = GetDataID(id, sharedData);
        return _frame->_renderSceneStorage.GetUniforms(dataID);
    }

    void RenderScene::StoreMaterial(Material& material)
    {
        StoreUniforms(material.GetID(), true, material.GetUniformValues());
    }

    void RenderScene::AddMeshData(uint64 id, Span<const Vector3> positions, Span<const uint32> indices,
        Optional<Span<const Vector3>> normals, Optional<Span<const Vector4>> colors,
        Optional<Span<const Vector4>> tangents, Optional<Span<const Vector2>> uvs)
    {
        _frame->EnsureDynamicMeshData(id, positions, indices, normals, colors, tangents, uvs);
    }

    void RenderScene::AddObject(uint64 id, uint64 layer, float order, Mesh& mesh, uint32 submeshIndex)
    {
        _frame->EnsureMeshData(mesh);

        auto submeshes = mesh.GetSubmeshes();
        Submesh drawSubmesh = submeshIndex < submeshes.size() ? submeshes[submeshIndex] : submeshes[0];

        _frame->_renderObjects.EmplaceBack(id, layer, mesh.GetID(), drawSubmesh, order);
        _renderObjectCount++;
    }

    void RenderScene::AddObject(uint64 id, uint64 layer, float order, Mesh& mesh, uint32 indexOffset, uint32 indexCount,
        int32 vertexOffset)
    {
        _frame->EnsureMeshData(mesh);

        AddObject(id, layer, order, mesh.GetID(), indexOffset, indexCount, vertexOffset);
    }

    void RenderScene::AddObject(uint64 id, uint64 layer, float order, uint64 meshID, uint32 indexOffset, uint32 indexCount,
        int32 vertexOffset)
    {
        Submesh submesh(indexOffset, indexCount, vertexOffset);

        _frame->_renderObjects.EmplaceBack(id, layer, meshID, submesh, order);
        _renderObjectCount++;
    }

    RenderObjectView RenderScene::GetRenderObjectView() const
    {
        return RenderObjectView(*this);
    }

    uint64 RenderScene::GetDataID(uint64 id, bool isShared) const
    {
        if (isShared)
            return id;

        return Math::CombineHashes(_id, id);
    }
} // Coco