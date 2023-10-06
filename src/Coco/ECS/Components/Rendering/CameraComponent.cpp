#include "ECSpch.h"
#include "CameraComponent.h"

#include <Coco/Rendering/RenderService.h>

using namespace Coco::Rendering;

namespace Coco::ECS
{
	CameraComponent::CameraComponent() :
		ClearColor(Color::Black),
		VerticalFOV(1.0),
		PerspectiveNearClip(0.1),
		PerspectiveFarClip(100.0),
		OrthoSize(1.0),
		OrthoNearClip(-1.0),
		OrthoFarClip(1.0),
		SampleCount(MSAASamples::One)
	{
		SetPerspectiveProjection(Math::DegToRad(90.0), 0.1, 100.0);
	}

	void CameraComponent::SetPerspectiveProjection(double verticalFOVRadians, double nearClip, double farClip)
	{
		VerticalFOV = verticalFOVRadians;
		PerspectiveNearClip = nearClip;
		PerspectiveFarClip = farClip;
		ProjectionType = CameraProjectionType::Perspective;
	}

	void CameraComponent::SetOrthographicProjection(double size, double nearClip, double farClip)
	{
		OrthoSize = size;
		OrthoNearClip = nearClip;
		OrthoFarClip = farClip;
		ProjectionType = CameraProjectionType::Orthographic;
	}

	Matrix4x4 CameraComponent::GetProjectionMatrix(double aspectRatio) const
	{
		Assert(RenderService::Get() != nullptr)

		switch (ProjectionType)
		{
		case CameraProjectionType::Perspective:
			return RenderService::Get()->GetPlatform().CreatePerspectiveProjection(VerticalFOV, aspectRatio, PerspectiveNearClip, PerspectiveFarClip);
		case CameraProjectionType::Orthographic:
			return RenderService::Get()->GetPlatform().CreateOrthographicProjection(OrthoSize, aspectRatio, OrthoNearClip, OrthoFarClip);
		default:
			return Matrix4x4::Identity;
		}
	}

	ViewFrustum CameraComponent::GetViewFrustum(double aspectRatio, const Vector3& position, const Quaternion& rotation) const
	{
		Vector3 forward = rotation * Vector3::Forward;
		Vector3 up = rotation * Vector3::Up;

		switch (ProjectionType)
		{
		case CameraProjectionType::Perspective:
			return ViewFrustum::CreatePerspective(position, forward, up, VerticalFOV, aspectRatio, PerspectiveNearClip, PerspectiveFarClip);
		case CameraProjectionType::Orthographic:
			return ViewFrustum::CreateOrthographic(position, forward, up, OrthoSize, aspectRatio, OrthoNearClip, OrthoFarClip);
		default:
			return ViewFrustum();
		}
	}

	double CameraComponent::GetNearClip() const
	{
		switch (ProjectionType)
		{
		case CameraProjectionType::Perspective:
			return PerspectiveNearClip;
		case CameraProjectionType::Orthographic:
			return OrthoNearClip;
		default:
			return 0.0;
		}
	}

	double CameraComponent::GetFarClip() const
	{
		switch (ProjectionType)
		{
		case CameraProjectionType::Perspective:
			return PerspectiveFarClip;
		case CameraProjectionType::Orthographic:
			return OrthoFarClip;
		default:
			return 0.0;
		}
	}
}