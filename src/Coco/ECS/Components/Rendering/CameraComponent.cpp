#include "ECSpch.h"
#include "CameraComponent.h"

#include <Coco/Rendering/RenderService.h>

using namespace Coco::Rendering;

namespace Coco::ECS
{
	CameraComponent::CameraComponent() :
		ClearColor(Color::Black),
		VerticalFOV(1.0),
		OrthoSize(1.0),
		NearClip(0.1),
		FarClip(100.0),
		SampleCount(MSAASamples::One)
	{
		SetPerspectiveProjection(Math::DegToRad(90.0), 0.1, 100.0);
	}

	void CameraComponent::SetPerspectiveProjection(double verticalFOVRadians, double nearClip, double farClip)
	{
		VerticalFOV = verticalFOVRadians;
		NearClip = nearClip;
		FarClip = farClip;
		ProjectionType = CameraProjectionType::Perspective;
	}

	void CameraComponent::SetOrthographicProjection(double size, double nearClip, double farClip)
	{
		OrthoSize = size;
		NearClip = nearClip;
		FarClip = farClip;
		ProjectionType = CameraProjectionType::Orthographic;
	}

	Matrix4x4 CameraComponent::GetProjectionMatrix(double aspectRatio) const
	{
		Assert(RenderService::Get() != nullptr)

		switch (ProjectionType)
		{
		case CameraProjectionType::Perspective:
			return RenderService::Get()->GetPlatform().CreatePerspectiveProjection(VerticalFOV, aspectRatio, NearClip, FarClip);
		case CameraProjectionType::Orthographic:
			return RenderService::Get()->GetPlatform().CreateOrthographicProjection(OrthoSize, aspectRatio, NearClip, FarClip);
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
			return ViewFrustum::CreatePerspective(position, forward, up, VerticalFOV, aspectRatio, NearClip, FarClip);
		case CameraProjectionType::Orthographic:
			return ViewFrustum::CreateOrthographic(position, forward, up, OrthoSize, aspectRatio, NearClip, FarClip);
		default:
			return ViewFrustum();
		}
	}
}