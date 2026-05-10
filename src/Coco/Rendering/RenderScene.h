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

    /// @brief Represents a scene that will be rendered
    class RenderScene
    {
        friend class RenderObjectView;
        friend class RenderObjectView::Iterator;

    public:
        RenderScene(RenderFrame* frame, uint64 id, const Sizei& frameSize);

        /// @brief Gets the ID of this render
        /// @return The ID
        uint64 GetID() const { return _id; }

        /// @brief Creates an orthographic projection matrix
        /// @param size The vertical size
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        /// @return The projection matrix
        Matrix4x4 CreateOrthographicProjection(float size, float nearClip, float farClip) const;

        /// @brief Creates an orthographic projection matrix
        /// @param left The position of the left side of the viewport
        /// @param right The position of the right side of the viewport
        /// @param bottom The position of the bottom side of the viewport
        /// @param top The position of the top side of the viewport
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        /// @return The projection matrix
        Matrix4x4 CreateOrthographicProjection(float left, float right, float bottom, float top, float nearClip, float farClip) const;

        /// @brief Creates a perspective projection matrix
        /// @param verticalFOV The vertical field of view, in radians
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        /// @return The projection matrix
        Matrix4x4 CreatePerspectiveProjection(float verticalFOV, float nearClip, float farClip) const;

        /// @brief Creates a perspective projection matrix
        /// @param verticalFOV The vertical field of view, in radians
        /// @param aspectRatio The aspect ratio of the projection
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        /// @return The projection matrix
        Matrix4x4 CreatePerspectiveProjection(float verticalFOV, float aspectRatio, float nearClip, float farClip) const;

        /// @brief Creates a primary orthographic camera for this scene
        /// @param position The camera position
        /// @param rotation The camera rotation
        /// @param size The vertical size
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        void SetOrthographicCamera(const Vector3& position, const Quaternion& rotation, float size, float nearClip, float farClip);

        /// @brief Creates a primary orthographic camera for this scene
        /// @param position The camera position
        /// @param target A point that the camera will look at
        /// @param up The upwards direction for the camera
        /// @param size The vertical size
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        void SetOrthographicCamera(const Vector3& position, const Vector3& target, const Vector3& up, float size, float nearClip, float farClip);

        /// @brief Creates a primary orthographic 2D camera for this scene
        /// @param position The camera position
        /// @param rotation The camera rotation
        /// @param size The vertical size
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        void SetOrthographicCamera(const Vector2& position, float rotation, float size, float nearClip, float farClip);

        /// @brief Creates a primary perspective camera for this scene
        /// @param position The camera position
        /// @param rotation The camera rotation
        /// @param verticalFOV The vertical field of view, in radians
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        void SetPerspectiveCamera(const Vector3& position, const Quaternion& rotation, float verticalFOV, float nearClip, float farClip);

        /// @brief Creates a primary perspective camera for this scene
        /// @param position The camera position
        /// @param target A point that the camera will look at
        /// @param up The upwards direction for the camera
        /// @param verticalFOV The vertical field of view, in radians
        /// @param nearClip The distance of the near clip plane
        /// @param farClip The distance of the far clip plane
        void SetPerspectiveCamera(const Vector3& position, const Vector3& target, const Vector3& up, float verticalFOV, float nearClip, float farClip);

        /// @brief Gets the position of this scene's primary camera
        /// @return The position of this scene's primary camera
        const Vector3& GetCameraPosition() const { return _viewPosition; }

        /// @brief Gets the rotation of this scene's primary camera
        /// @return The rotation of this scene's primary camera
        const Quaternion& GetCameraRotation() const { return _viewRotation; }

        /// @brief Gets the view matrix of this scene's primary camera
        /// @return The view matrix of this scene's primary camera
        const Matrix4x4& GetViewMatrix() const { return _viewMatrix; }

        /// @brief Gets the projection matrix of this scene's primary camera
        /// @return The projection matrix of this scene's primary camera
        const Matrix4x4& GetProjectionMatrix() const { return _projectionMatrix; }

        /// @brief Stores arbitrary render data for this scene. IDs are unique to the type of data being stored, so the same IDs can be used as long as different types of data are being stored
        /// @tparam DataType The type of data
        /// @param id The ID of the data
        /// @param sharedData If true, the data can be shared across renders this frame
        /// @param data The data to store
        template<typename DataType>
        void StoreData(uint64 id, bool sharedData, const DataType& data)
        {
            uint64 dataID = GetDataID(id, sharedData);
            _frame->GetSceneStorage().Store(dataID, data);
        }

        /// @brief Determines if render data with the given ID exists for this scene
        /// @tparam DataType The type of data
        /// @param id The ID of the data
        /// @param sharedData If true, the data can be shared across renders this frame
        /// @return True if the given data exists
        template<typename DataType>
        bool HasData(uint64 id, bool sharedData) const
        {
            uint64 dataID = GetDataID(id, sharedData);
            return _frame->GetSceneStorage().Has<DataType>(dataID);
        }

        /// @brief Gets render data previously stored for this scene
        /// @tparam DataType The type of data
        /// @param id The ID of the data
        /// @param sharedData If true, the data can be shared across renders this frame
        /// @return The data
        template<typename DataType>
        const DataType* GetData(uint64 id, bool sharedData) const
        {
            uint64 dataID = GetDataID(id, sharedData);
            return _frame->GetSceneStorage().Get<DataType>(dataID);
        }

        /// @brief Gets a texture resource via its ID
        /// @param id The ID of the texture resource
        /// @return The texture
        SharedPtr<Texture> GetTexture(uint64 id) const;

        /// @brief Stores shader uniforms for this scene. IDs do not conflict with IDs for arbitrary render data stored with StoreData()
        /// @param id The ID of the data
        /// @param sharedData If true, the data can be shared across renders this frame
        /// @param uniforms The uniforms to store
        void StoreUniforms(uint64 id, bool sharedData, Span<const ShaderUniformValue> uniforms);

        /// @brief Determines if shader uniforms with the given ID exists for this scene
        /// @param id The ID of the data
        /// @param sharedData If true, the data can be shared across renders this frame
        /// @return True if the data exists
        bool HasUniforms(uint64 id, bool sharedData) const;

        /// @brief Gets shader uniforms previously stored for this scene
        /// @param id The ID of the data
        /// @param sharedData If true, the data can be shared across renders this frame
        /// @return The shader uniforms
        Span<const ShaderUniformValue> GetUniforms(uint64 id, bool sharedData) const;

        /// @brief Stores the given materials uniforms for this scene
        /// @param material The material
        void StoreMaterial(Material& material);

        /// @brief Adds raw mesh data for this scene
        /// @param id The mesh ID
        /// @param positions The vertex positions
        /// @param indices The vertex indices
        /// @param normals The vertex normals (optional)
        /// @param colors The vertex colors (optional)
        /// @param tangents The vertex tangents (optional)
        /// @param uvs The vertex UVs (optional)
        void AddMeshData(uint64 id, Span<const Vector3> positions, Span<const uint32> indices,
            Optional<Span<const Vector3>> normals, Optional<Span<const Vector4>> colors,
            Optional<Span<const Vector4>> tangents, Optional<Span<const Vector2>> uvs);

        /// @brief Adds a RenderObject for this scene
        /// @param id The object ID
        /// @param layer The object's layer
        /// @param mesh The mesh to render the object with
        /// @param submeshIndex The index of the submesh to render the object with
        void AddObject(uint64 id, uint64 layer, Mesh& mesh, uint32 submeshIndex = 0);

        /// @brief Adds a RenderObject for this scene
        /// @param id The object ID
        /// @param layer The object's layer
        /// @param mesh The mesh to render the object with
        /// @param indexOffset The offset in the vertex buffer of the first index to render
        /// @param indexCount The number of indices to render
        /// @param vertexOffset An offset to apply to each vertex index
        void AddObject(uint64 id, uint64 layer, Mesh& mesh, uint32 indexOffset, uint32 indexCount, int32 vertexOffset = 0);

        /// @brief Adds a RenderObject for this scene
        /// @param id The object ID
        /// @param layer The object's layer
        /// @param meshID The ID of the mesh
        /// @param indexOffset The offset in the vertex buffer of the first index to render
        /// @param indexCount The number of indices to render
        /// @param vertexOffset An offset to apply to each vertex index
        void AddObject(uint64 id, uint64 layer, uint64 meshID, uint32 indexOffset, uint32 indexCount, int32 vertexOffset = 0);

        /// @brief Gets a view to iterate over this scene's RenderObjects
        /// @return A view over this scene's RenderObjects
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

        /// @brief Computes an ID for render data for this scene
        /// @param id The ID
        /// @param isShared If true, the ID will be consistent across renders for this frame
        /// @return The data ID
        uint64 GetDataID(uint64 id, bool isShared) const;
    };
} // Coco

#endif //COCOENGINE_RENDERSCENE_H