#pragma once

#include "Vector.h"

namespace Coco
{
	struct BoundingBox;

	/// @brief Types of intersections between a plane and a volume
	enum class PlaneVolumeIntersection
	{
		OnOppositeNormalSide,
		Intersects,
		OnNormalSide
	};

	/// @brief Represents an infinite plane that faces a direction
	struct Plane
	{
		/// @brief The direction that this plane faces
		Vector3 Normal;

		/// @brief The distance that this plane is along its normal from the world origin
		double Distance;

		Plane();
		Plane(const Vector3& normal, double distance);
		Plane(const Vector3& origin, const Vector3& normal);
		Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2);

		/// @brief Gets a point closest to the given point that lies on this plane
		/// @param point The point
		/// @return A point that lies on this plane that is closest to the given point
		Vector3 GetClosestPoint(const Vector3& point) const;

		/// @brief Gets the distance the given point is from the closest point on this plane
		/// @param point The point
		/// @return The distance the point is from the closest point on this plane
		double GetDistance(const Vector3& point) const;

		/// @brief Determines if this plane faces the given point
		/// @param point The point
		/// @return True if this plane faces the given point
		bool IsOnNormalSide(const Vector3& point) const;

		/// @brief Determines how a bounding box intersects with this plane
		/// @param box The bounding box
		/// @return The intersection type
		PlaneVolumeIntersection CheckIntersection(const BoundingBox& box) const;
	};
}