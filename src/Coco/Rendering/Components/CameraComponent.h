#pragma once

#include <Coco/Core/Scene/Components/EntityComponent.h>

#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Size.h>
#include "CameraComponentTypes.h"

namespace Coco::Rendering
{
	/// @brief A camera component that can render a scene from a perspective
	class COCOAPI CameraComponent : public EntityComponent
	{
	private:
		ProjectionType _projectionType = ProjectionType::Custom;

		Matrix4x4 _projectionMatrix = Matrix4x4::Identity;
		Matrix4x4 _viewMatrix = Matrix4x4::Identity;

		double _nearClipDistance = 0.1;
		double _farClipDistance = 100.0;

		double _perspectiveFieldOfView = 90.0;
		double _orthographicSize = 10.0;
		double _aspectRatio = 1.0;

		bool _isProjectionMatrixDirty = true;

	public:
		CameraComponent() = default;
		virtual ~CameraComponent() = default;

		/// @brief Gets the type of projection that this camera is using
		/// @return The type of projection that this camera is using
		ProjectionType GetProjectionType() const noexcept { return _projectionType; }

		/// @brief Sets this camera to use a perspective projection
		/// @param fieldOfView The vertical field of view (in degrees)
		/// @param aspectRatio The aspect ratio
		/// @param nearClipDistance The near clip distance
		/// @param farClipDistance The far clip distance
		void SetPerspectiveProjection(double fieldOfView, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept;

		/// @brief Sets this camera to use an orthographic projection
		/// @param size The size of the projection
		/// @param aspectRatio The aspect ratio
		/// @param nearClipDistance The near clip distance
		/// @param farClipDistance The far clip distance
		void SetOrthographicProjection(double size, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept;

		/// @brief Sets this camera to use a custom projection matrix
		/// @param projection The custom projection matrix
		void SetCustomProjection(const Matrix4x4& projection) noexcept;

		/// @brief Gets this camera's current projection matrix
		/// @return The current projection matrix
		Matrix4x4 GetProjectionMatrix() noexcept;

		/// @brief Sets this camera's current view matrix
		/// @param view The view matrix
		void SetViewMatrix(const Matrix4x4& view) noexcept { _viewMatrix = view; }

		/// @brief Gets this camera's current view matrix
		/// @return The current view matrix
		Matrix4x4 GetViewMatrix() const noexcept { return _viewMatrix; }

		/// @brief Sets this camera's aspect ratio
		/// @param aspectRatio The new aspect ratio
		void SetAspectRatio(double aspectRatio) noexcept;

		/// @brief Gets this camera's current aspect ratio
		/// @return This camera's current aspect ratio
		double GetAspectRatio() const noexcept { return _aspectRatio; }

		/// @brief Sets this camera's near clip plane distance
		/// @param nearClipDistance The new near clip plane distance
		void SetNearClipDistance(double nearClipDistance) noexcept;

		/// @brief Gets this camera's current near clip plane distance
		/// @return This camera's current near clip plane distance
		double GetNearClipDistance() const noexcept { return _nearClipDistance; }

		/// @brief Sets this camera's far clip plane distance
		/// @param farClipDistance The new far clip plane distance
		void SetFarClipDistance(double farClipDistance) noexcept;

		/// @brief Gets this camera's current far clip plane distance
		/// @return This camera's current far clip plane distance
		double GetFarClipDistance() const noexcept { return _farClipDistance; }

	private:
		/// @brief Updates the internal projection matrix based on the projection type
		/// @return 
		void UpdateProjectionMatrix() noexcept;
	};
}