#pragma once

#include "Size.h"
#include "Plane.h"

namespace Coco
{
	/// @brief Defines a view frustrum clipped by 6 planes
	struct ViewFrustum
	{
		/// @brief The left plane
		Plane Left;

		/// @brief The right plane
		Plane Right;

		/// @brief The bottom plane
		Plane Bottom;

		/// @brief The top plane
		Plane Top;

		/// @brief The near plane
		Plane Near;

		/// @brief The far plane
		Plane Far;

		ViewFrustum(
			const Vector3& origin, 
			const Vector3& direction,
			const Vector3& up,
			const Vector3& nearOffset,
			const Size& nearSize,
			const Vector3& farOffset,
			const Size& farSize);

		/// @brief Creates a perspective view frustum
		/// @param origin The eye position
		/// @param direction The look direction
		/// @param up The up direction
		/// @param verticalFOVRadians The vertical field of view, in radians
		/// @param aspectRatio The aspect ratio
		/// @param nearClip The distance of the near clip plane
		/// @param farClip The distance of the far clip plane
		/// @return A view frustum
		static ViewFrustum CreatePerspective(
			const Vector3& origin, 
			const Vector3& direction,
			const Vector3& up,
			double verticalFOVRadians,
			double aspectRatio, 
			double nearClip, 
			double farClip);

		/// @brief Creates an orthographic view frustum
		/// @param origin The eye position
		/// @param direction The look direction
		/// @param up The up direction
		/// @param size The orthographic size
		/// @param aspectRatio The aspect ratio
		/// @param nearClip The distance of the near clip plane
		/// @param farClip The distance of the far clip plane
		/// @return A view frustum
		static ViewFrustum CreateOrthographic(
			const Vector3& origin, 
			const Vector3& direction,
			const Vector3& up,
			double size, 
			double aspectRatio,
			double nearClip,
			double farClip);

		/// @brief Creates an orthographic view frustum
		/// @param origin The eye position
		/// @param direction The look direction
		/// @param up The up direction
		/// @param left The left side of the view from the eye position
		/// @param right The right side of the view from the eye position
		/// @param bottom The bottom side of the view from the eye position
		/// @param top The up side of the view from the eye position
		/// @param nearClip The distance of the near clip plane
		/// @param farClip The distance of the far clip plane
		/// @return A view frustum
		static ViewFrustum CreateOrthographic(
			const Vector3& origin,
			const Vector3& direction,
			const Vector3& up,
			double left,
			double right,
			double bottom,
			double top,
			double nearClip,
			double farClip);

		/// @brief Determines if a point is within this frustum
		/// @param point The point
		/// @return True if the point is contained within this frustum
		bool IsInside(const Vector3& point) const;
	};
}