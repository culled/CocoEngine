#pragma once

#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/ViewFrustum.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Rendering/Graphics/GraphicsPipelineTypes.h>

namespace Coco::ECS
{
    /// @brief Types of camera projections
    enum class CameraProjectionType
    {
        Perspective,
        Orthographic
    };

    /// @brief A component that allows a scene to be rendered from its perspective
    struct CameraComponent
    {
        /// @brief The color that will be used for clearing the frame
        Color ClearColor;

        /// @brief The type of projection that this camera has
        CameraProjectionType ProjectionType;

        /// @brief The distance to the near clip plane in perspective mode
        double PerspectiveNearClip;

        /// @brief The distance to the far clip plane in perspective mode
        double PerspectiveFarClip;

        /// @brief The vertical field of view, in radians
        double VerticalFOV;

        /// @brief The distance to the near clip plane in orthographic mode
        double OrthoNearClip;

        /// @brief The distance to the far clip plane in orthographic mode
        double OrthoFarClip;

        /// @brief The vertical size of the orthographic frustum
        double OrthoSize;

        /// @brief The number of msaa samples
        Rendering::MSAASamples SampleCount;

        CameraComponent();

        /// @brief Sets this camera to use a perspective projection
        /// @param verticalFOVRadians The vertical field of view, in radians
        /// @param nearClip The distance to the near clip plane
        /// @param farClip The distance to the far clip plane
        void SetPerspectiveProjection(double verticalFOVRadians, double nearClip, double farClip);

        /// @brief Sets this camera to use an orthographic projection
        /// @param size The vertical size of the orthographic frustum
        /// @param nearClip The distance to the near clip plane
        /// @param farClip The distance to the far clip plane
        void SetOrthographicProjection(double size, double nearClip, double farClip);

        /// @brief Gets the projection matrix for this camera
        /// @param aspectRatio The aspect ratio of the matrix
        /// @return The projection matrix
        Matrix4x4 GetProjectionMatrix(double aspectRatio) const;

        /// @brief Gets the view frustum for this camera
        /// @param aspectRatio The aspect ratio of the frustum
        /// @param position The origin of the frustum
        /// @param rotation The rotation of the camera
        /// @return The view frustum
        ViewFrustum GetViewFrustum(double aspectRatio, const Vector3& position, const Quaternion& rotation) const;

        /// @brief Gets the near clip distance for this camera
        /// @return The near clip distance
        double GetNearClip() const;

        /// @brief Gets the far clip distance for this camera
        /// @return The far clip distance
        double GetFarClip() const;
    };
}