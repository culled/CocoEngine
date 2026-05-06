//
// Created by cullen on 4/2/26.
//

#include "CameraComponent.h"

namespace Coco
{
    DEFINE_RTTI_TYPE(CameraComponent, EntityComponent);

    PerspectiveCameraParameters::PerspectiveCameraParameters() :
        VerticalFOV(Math::DegToRad(90.0f)),
        NearClip(0.1f),
        FarClip(100.0f)
    {}

    OrthographicCameraParameters::OrthographicCameraParameters() :
        Size(5.0f),
        NearClip(0.0f),
        FarClip(100.0f)
    {}

    CameraComponent::CameraComponent(const UUID& ownerEntityID) :
        EntityComponent(ownerEntityID),
        PerspectiveCamera(),
        OrthographicCamera(),
        ClearColor(Color::Black),
        Type(ProjectionType::Perspective)
    {}

    void CameraComponent::SetPerspectiveProjection(float verticalFOV, float nearClip, float farClip)
    {
        PerspectiveCamera.VerticalFOV = verticalFOV;
        PerspectiveCamera.NearClip = nearClip;
        PerspectiveCamera.FarClip = farClip;
        Type = ProjectionType::Perspective;
    }

    void CameraComponent::SetOrthographicProjection(float size, float nearClip, float farClip)
    {
        OrthographicCamera.Size = size;
        OrthographicCamera.NearClip = nearClip;
        OrthographicCamera.FarClip = farClip;
        Type = ProjectionType::Orthographic;
    }
} // Coco