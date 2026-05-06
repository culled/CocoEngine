//
// Created by cullen on 4/2/26.
//

#ifndef COCOENGINE_CAMERACOMPONENT_H
#define COCOENGINE_CAMERACOMPONENT_H
#include "Coco/Core/Types/Color.h"
#include "Coco/ECS/EntityComponent.h"

namespace Coco
{
    struct PerspectiveCameraParameters
    {
        float VerticalFOV;
        float NearClip;
        float FarClip;

        PerspectiveCameraParameters();
    };

    struct OrthographicCameraParameters
    {
        float Size;
        float NearClip;
        float FarClip;

        OrthographicCameraParameters();
    };

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

        PerspectiveCameraParameters PerspectiveCamera;
        OrthographicCameraParameters OrthographicCamera;
        Color ClearColor;
        ProjectionType Type;

        CameraComponent(const UUID& ownerEntityID);

        void SetPerspectiveProjection(float verticalFOV, float nearClip, float farClip);
        void SetOrthographicProjection(float size, float nearClip, float farClip);
    };
} // Coco

#endif //COCOENGINE_CAMERACOMPONENT_H