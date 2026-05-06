//
// Created by cullen on 3/7/26.
//

#ifndef COCOENGINE_RENDERSCENE_H
#define COCOENGINE_RENDERSCENE_H
#include <Coco/Core/Math/Vector3.h>
#include <Coco/Core/Math/Quaternion.h>
#include <Coco/Core/Memory/Ptrs.h>

#include "RenderObjectView.h"
#include "RenderSceneTypes.h"

#include "Coco/Core/Math/Matrix4x4.h"

#include "Graphics/RenderFrame.h"
#include "Graphics/ShaderUniformValue.h"

namespace Coco
{
    class Material;
    class Mesh;
    class Shader;
    class Texture;

    class RenderScene
    {
        friend class RenderObjectView;
        friend class RenderObjectView::Iterator;

    public:
        RenderScene(RenderFrame* frame, uint64 id, const Sizei& frameSize);

        uint64 GetID() const { return _id; }
        Matrix4x4 CreateOrthographicProjection(float size, float nearClip, float farClip) const;
        Matrix4x4 CreateOrthographicProjection(float left, float right, float bottom, float top, float nearClip, float farClip) const;
        Matrix4x4 CreatePerspectiveProjection(float verticalFOV, float nearClip, float farClip) const;
        Matrix4x4 CreatePerspectiveProjection(float verticalFOV, float aspectRatio, float nearClip, float farClip) const;

        void SetOrthographicCamera(const Vector3& position, const Quaternion& rotation, float size, float nearClip, float farClip);
        void SetOrthographicCamera(const Vector3& position, const Vector3& target, const Vector3& up, float size, float nearClip, float farClip);
        void SetOrthographicCamera(const Vector2& position, float rotation, float size, float nearClip, float farClip);
        void SetPerspectiveCamera(const Vector3& position, const Quaternion& rotation, float verticalFOV, float nearClip, float farClip);
        void SetPerspectiveCamera(const Vector3& position, const Vector3& target, const Vector3& up, float verticalFOV, float nearClip, float farClip);

        const Vector3& GetCameraPosition() const { return _viewPosition; }
        const Quaternion& GetCameraRotation() const { return _viewRotation; }
        const Matrix4x4& GetViewMatrix() const { return _viewMatrix; }
        const Matrix4x4& GetProjectionMatrix() const { return _projectionMatrix; }

        template<typename DataType>
        void StoreData(uint64 id, bool sharedData, const DataType& data)
        {
            uint64 dataID = GetDataID(id, sharedData);
            _frame->GetSceneStorage().Store(dataID, data);
        }

        template<typename DataType>
        bool HasData(uint64 id, bool sharedData) const
        {
            uint64 dataID = GetDataID(id, sharedData);
            return _frame->GetSceneStorage().Has<DataType>(dataID);
        }

        template<typename DataType>
        const DataType* GetData(uint64 id, bool sharedData) const
        {
            uint64 dataID = GetDataID(id, sharedData);
            return _frame->GetSceneStorage().Get<DataType>(dataID);
        }

        SharedPtr<Texture> GetTexture(uint64 id) const;

        void StoreUniforms(uint64 id, bool sharedData, Span<const ShaderUniformValue> uniforms);
        bool HasUniforms(uint64 id, bool sharedData) const;
        Span<const ShaderUniformValue> GetUniforms(uint64 id, bool sharedData) const;
        void StoreMaterial(Material& material);

        void AddMeshData(uint64 id, Span<const Vector3> positions, Span<const uint32> indices,
            Optional<Span<const Vector3>> normals, Optional<Span<const Vector4>> colors,
            Optional<Span<const Vector4>> tangents, Optional<Span<const Vector2>> uvs);
        void AddObject(uint64 id, uint64 layer, Mesh& mesh, uint32 submeshIndex = 0);
        void AddObject(uint64 id, uint64 layer, Mesh& mesh, uint32 indexOffset, uint32 indexCount, int32 vertexOffset = 0);
        void AddObject(uint64 id, uint64 layer, uint64 meshID, uint32 indexOffset, uint32 indexCount, int32 vertexOffset = 0);

        RenderObjectView GetRenderObjectView() const;

    private:
        RenderFrame* _frame;
        uint64 _id;
        Sizei _frameSize;

        Vector3 _viewPosition;
        Quaternion _viewRotation;
        Matrix4x4 _viewMatrix;
        Matrix4x4 _projectionMatrix;
        uint64 _firstRenderObjectIndex;
        uint64 _renderObjectCount;

    private:
        uint64 GetDataID(uint64 id, bool isShared) const;
    };
} // Coco

#endif //COCOENGINE_RENDERSCENE_H