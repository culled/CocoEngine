#pragma once

#include <Coco/Core/Core.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/Size.h>

#include "CameraComponentTypes.h"

namespace Coco
{
	/// <summary>
	/// A camera component
	/// </summary>
	class COCOAPI CameraComponent
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

		/// <summary>
		/// Gets the type of projection that this camera is using
		/// </summary>
		/// <returns>The type of projection that this camera is using</returns>
		ProjectionType GetProjectionType() const noexcept { return _projectionType; }

		/// <summary>
		/// Sets this camera to use a perspective projection
		/// </summary>
		/// <param name="fieldOfView">The horizontal field of view (in degrees)</param>
		/// <param name="aspectRatio">The aspect ratio</param>
		/// <param name="nearClipDistance">The near clip distance</param>
		/// <param name="farClipDistance">The far clip distance</param>
		void SetPerspectiveProjection(double fieldOfView, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept;

		/// <summary>
		/// Sets this camera to use an orthographic projection
		/// </summary>
		/// <param name="size">The size of the projection</param>
		/// <param name="aspectRatio">The aspect ratio</param>
		/// <param name="nearClipDistance">The near clip distance</param>
		/// <param name="farClipDistance">The far clip distance</param>
		void SetOrthographicProjection(double size, double aspectRatio, double nearClipDistance, double farClipDistance) noexcept;

		/// <summary>
		/// Sets this camera to use a custom projection matrix
		/// </summary>
		/// <param name="projection">The custom projection matrix</param>
		void SetCustomProjection(const Matrix4x4& projection) noexcept;

		/// <summary>
		/// Gets this camera's current projection matrix
		/// </summary>
		/// <returns>The current projection matrix</returns>
		Matrix4x4 GetProjectionMatrix() noexcept;

		void SetViewMatrix(const Matrix4x4& view) noexcept { _viewMatrix = view; }
		Matrix4x4 GetViewMatrix() const noexcept { return _viewMatrix; }

		/// <summary>
		/// Sets this camera's aspect ratio
		/// </summary>
		/// <param name="aspectRatio">The new aspect ratio</param>
		void SetAspectRatio(double aspectRatio) noexcept;

		/// <summary>
		/// Gets this camera's current aspect ratio
		/// </summary>
		/// <returns>This camera's current aspect ratio</returns>
		double GetAspectRatio() const noexcept { return _aspectRatio; }

		/// <summary>
		/// Sets this camera's near clip plane distance
		/// </summary>
		/// <param name="nearClipDistance">The new near clip plane distance</param>
		void SetNearClipDistance(double nearClipDistance) noexcept;

		/// <summary>
		/// Gets this camera's current near clip plane distance
		/// </summary>
		/// <returns>This camera's current near clip plane distance</returns>
		double GetNearClipDistance() const noexcept { return _nearClipDistance; }

		/// <summary>
		/// Sets this camera's far clip plane distance
		/// </summary>
		/// <param name="farClipDistance">The new far clip plane distance</param>
		void SetFarClipDistance(double farClipDistance) noexcept;

		/// <summary>
		/// Gets this camera's current far clip plane distance
		/// </summary>
		/// <returns>This camera's current far clip plane distance</returns>
		double GetFarClipDistance() const noexcept { return _farClipDistance; }

	private:
		/// <summary>
		/// Updates the internal projection matrix based on the projection type
		/// </summary>
		void UpdateProjectionMatrix() noexcept;
	};
}