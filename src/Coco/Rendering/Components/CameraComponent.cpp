#include "CameraComponent.h"

#include "../Pipeline/RenderPipeline.h"
#include "../Graphics/Resources/Image.h"

namespace Coco::Rendering
{
	CameraComponent::CameraComponent(SceneEntity* entity) : EntityComponent(entity)
	{}

	void CameraComponent::SetPerspectiveProjection(double fieldOfView, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept
	{
		_projectionType = ProjectionType::Perspective;
		_perspectiveFieldOfView = fieldOfView;
		_nearClipDistance = nearClipDistance;
		_farClipDistance = farClipDistance;

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetOrthographicProjection(double size, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept
	{
		_projectionType = ProjectionType::Orthographic;
		_orthographicSize = size;
		_nearClipDistance = nearClipDistance;
		_farClipDistance = farClipDistance;

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetCustomProjection(const Matrix4x4& projection) noexcept
	{
		_projectionType = ProjectionType::Custom;
		_projectionMatrix = projection;

		_isProjectionMatrixDirty = false;
	}

	Matrix4x4 CameraComponent::GetProjectionMatrix() noexcept
	{
		if (_isProjectionMatrixDirty)
			UpdateProjectionMatrix();

		return _projectionMatrix;
	}

	void CameraComponent::SetAspectRatio(double aspectRatio) noexcept
	{
		if (Math::Approximately(aspectRatio, _aspectRatio))
			return;

		_aspectRatio = aspectRatio;

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetNearClipDistance(double nearClipDistance) noexcept
	{
		if (Math::Approximately(nearClipDistance, _nearClipDistance))
			return;

		_nearClipDistance = Math::Max(nearClipDistance, Math::Epsilon);

		_isProjectionMatrixDirty = true;
	}

	void CameraComponent::SetFarClipDistance(double farClipDistance) noexcept
	{
		if (Math::Approximately(farClipDistance, _farClipDistance))
			return;

		_farClipDistance = Math::Max(farClipDistance, Math::Epsilon);

		_isProjectionMatrixDirty = true;
	}

	List<WeakManagedRef<Image>> CameraComponent::GetRenderTargets(const Ref<RenderPipeline>& pipeline)
	{
		// TODO: build this from the pipeline
		return _renderTargetOverrides;
	}

	void CameraComponent::UpdateProjectionMatrix() noexcept
	{
		if (_projectionType == ProjectionType::Custom || !_isProjectionMatrixDirty)
			return;
		
		if (_projectionType == ProjectionType::Perspective)
		{
			_projectionMatrix = Matrix4x4::CreatePerspectiveProjection(
				Math::Deg2Rad(_perspectiveFieldOfView), 
				_aspectRatio, 
				_nearClipDistance, 
				_farClipDistance);
		}
		else
		{
			_projectionMatrix = Matrix4x4::CreateOrthographicProjection(
				_orthographicSize, 
				_aspectRatio, 
				_nearClipDistance, 
				_farClipDistance);
		}

		_isProjectionMatrixDirty = false;
	}
}