#include "ECSpch.h"
#include "CameraComponent.h"
#include "CameraSystem.h"

#include <Coco/Rendering/RenderService.h>

using namespace Coco::Rendering;

namespace Coco::ECS
{
	CameraComponent::CameraComponent(const Entity& owner) :
		EntityComponent(owner),
		_clearColor(Color::Black),
		_perspectiveFOV(1.0),
		_perspectiveNearClip(0.1),
		_perspectiveFarClip(100.0),
		_orthoSize(1.0),
		_orthoNearClip(-1.0),
		_orthoFarClip(1.0),
		_sampleCount(MSAASamples::One),
		_priority(0)
	{
		SetPerspectiveProjection(Math::DegToRad(90.0), 0.1, 100.0);
	}

	void CameraComponent::SetPerspectiveProjection(double verticalFOVRadians, double nearClip, double farClip)
	{
		_perspectiveFOV = verticalFOVRadians;
		_perspectiveNearClip = nearClip;
		_perspectiveFarClip = farClip;
		_projectionType = CameraProjectionType::Perspective;
	}

	void CameraComponent::SetOrthographicProjection(double size, double nearClip, double farClip)
	{
		_orthoSize = size;
		_orthoNearClip = nearClip;
		_orthoFarClip = farClip;
		_projectionType = CameraProjectionType::Orthographic;
	}

	Matrix4x4 CameraComponent::GetProjectionMatrix(double aspectRatio) const
	{
		Assert(RenderService::Get() != nullptr)

		switch (_projectionType)
		{
		case CameraProjectionType::Perspective:
			return RenderService::Get()->GetPlatform().CreatePerspectiveProjection(_perspectiveFOV, aspectRatio, _perspectiveNearClip, _perspectiveFarClip);
		case CameraProjectionType::Orthographic:
			return RenderService::Get()->GetPlatform().CreateOrthographicProjection(_orthoSize, aspectRatio, _orthoNearClip, _orthoFarClip);
		default:
			return Matrix4x4::Identity;
		}
	}

	ViewFrustum CameraComponent::GetViewFrustum(double aspectRatio, const Vector3& position, const Quaternion& rotation) const
	{
		Vector3 forward = rotation * Vector3::Forward;
		Vector3 up = rotation * Vector3::Up;

		switch (_projectionType)
		{
		case CameraProjectionType::Perspective:
			return ViewFrustum::CreatePerspective(position, forward, up, _perspectiveFOV, aspectRatio, _perspectiveNearClip, _perspectiveFarClip);
		case CameraProjectionType::Orthographic:
			return ViewFrustum::CreateOrthographic(position, forward, up, _orthoSize, aspectRatio, _orthoNearClip, _orthoFarClip);
		default:
			return ViewFrustum();
		}
	}

	void CameraComponent::SetProjectionType(CameraProjectionType projectionType)
	{
		_projectionType = projectionType;
	}

	void CameraComponent::SetPriority(int priority)
	{
		_priority = priority;
	}

	void CameraComponent::SetNearClip(double nearClip)
	{
		switch (_projectionType)
		{
		case CameraProjectionType::Perspective:
			_perspectiveNearClip = nearClip;
			break;
		case CameraProjectionType::Orthographic:
			_orthoNearClip = nearClip;
			break;
		default:
			break;
		}
	}

	double CameraComponent::GetNearClip() const
	{
		switch (_projectionType)
		{
		case CameraProjectionType::Perspective:
			return _perspectiveNearClip;
		case CameraProjectionType::Orthographic:
			return _orthoNearClip;
		default:
			return 0.0;
		}
	}

	void CameraComponent::SetFarClip(double farClip)
	{
		switch (_projectionType)
		{
		case CameraProjectionType::Perspective:
			_perspectiveFarClip = farClip;
			break;
		case CameraProjectionType::Orthographic:
			_orthoFarClip = farClip;
			break;
		default:
			break;
		}
	}

	double CameraComponent::GetFarClip() const
	{
		switch (_projectionType)
		{
		case CameraProjectionType::Perspective:
			return _perspectiveFarClip;
		case CameraProjectionType::Orthographic:
			return _orthoFarClip;
		default:
			return 0.0;
		}
	}

	void CameraComponent::SetClearColor(const Color& clearColor)
	{
		_clearColor = clearColor;
	}

	void CameraComponent::SetPerspectiveFOV(double verticalFOVRadians)
	{
		_perspectiveFOV = verticalFOVRadians;
	}

	void CameraComponent::SetOrthographicSize(double verticalSize)
	{
		_orthoSize = verticalSize;
	}

	void CameraComponent::SetSampleCount(Rendering::MSAASamples sampleCount)
	{
		_sampleCount = sampleCount;
	}

	void CameraComponent::Render(std::span<Ref<Image>> framebuffers, RenderPipeline& pipeline, std::optional<GlobalShaderUniformLayout> layoutOverride)
	{
		CameraSystem::Render(*this, framebuffers, pipeline, layoutOverride);
	}

	void CameraComponent::Render(Ref<GraphicsPresenter> presenter, RenderPipeline& pipeline, std::optional<GlobalShaderUniformLayout> layoutOverride)
	{
		CameraSystem::Render(*this, presenter, pipeline, layoutOverride);
	}
}