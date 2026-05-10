//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_CAMERACOMPONENT_H
#define COCOENGINE_CAMERACOMPONENT_H
#include "Coco/Core/Types/Color.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    /// @brief Parameters for a perspective camera
    struct PerspectiveCameraParameters
    {
        /// @brief The vertical field of view (in radians)
        float VerticalFOV;

        /// @brief The near clip distance
        float NearClip;

        /// @brief The far clip distance
        float FarClip;

        PerspectiveCameraParameters();
    };

    /// @brief Parameters for an orthographic camera
    struct OrthographicCameraParameters
    {
        /// @brief The vertical size of the camera
        float Size;

        /// @brief The near clip distance
        float NearClip;

        /// @brief The far clip distance
        float FarClip;

        OrthographicCameraParameters();
    };

    /// @brief An EntityComponent that defines camera parameters
    struct CameraComponent : public EntityComponent
    {
        DECLARE_RTTI_TYPE(CameraComponent)

    public:
        /// @brief Types of camera projections
        enum class ProjectionType : uint8
        {
            Perspective,
            Orthographic
        };

        /// @brief Perspective camera parameters
        PerspectiveCameraParameters PerspectiveCamera;

        /// @brief Orthographic camera parameters
        OrthographicCameraParameters OrthographicCamera;

        /// @brief The color to clear the background to
        Color ClearColor;

        /// @brief The type of projection to use
        ProjectionType Type;

        CameraComponent(const UUID& ownerEntityID);

        /// @brief
        /// @param verticalFOV The vertical field of view, in radians
        /// @param nearClip The near clip distance
        /// @param farClip The far clip distance
        void SetPerspectiveProjection(float verticalFOV, float nearClip, float farClip);

        /// @brief
        /// @param size The vertical size of the camera
        /// @param nearClip The near clip distance
        /// @param farClip The far clip distance
        void SetOrthographicProjection(float size, float nearClip, float farClip);
    };
} // Coco

#endif //COCOENGINE_CAMERACOMPONENT_H