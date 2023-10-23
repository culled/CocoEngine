#include "Corepch.h"
#include "Plane.h"
#include "BoundingBox.h"

namespace Coco
{
	Plane::Plane() :
		Plane(Vector3::Up, 0.0)
	{}

	Plane::Plane(const Vector3& normal, double distance) :
		Normal(normal.Normalized()),
		Distance(distance)
	{}

	Plane::Plane(const Vector3& origin, const Vector3& normal) :
		Normal(normal.Normalized()),
		Distance(normal.Dot(origin))
	{
	}

	Plane::Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2)
	{
		Vector3 origin = (p0 + p1 + p2) / 3.0;
		Vector3 a = (p1 - p0).Normalized();
		Vector3 b = (p2 - p1).Normalized();
		Normal = b.Cross(a).Normalized();
		Distance = Normal.Dot(origin);
	}

	Vector3 Plane::GetClosestPoint(const Vector3& point) const
	{
		Vector3 p = point.Project(Normal) + Normal * Distance;
		return point - p;
	}

	double Plane::GetDistance(const Vector3& point) const
	{
		return Vector3::DistanceBetween(GetClosestPoint(point), point);
	}

	bool Plane::IsOnNormalSide(const Vector3& point) const
	{
		Vector3 p = point.Project(Normal);

		return p.Dot(Normal) > Distance;
	}

	PlaneVolumeIntersection Plane::CheckIntersection(const BoundingBox& box) const
	{
		// https://www.gamedev.net/forums/topic/512123-fast--and-correct-frustum---aabb-intersection/

		Vector3 min;
		Vector3 max;

		// Determin the minimum and maximum points to use base on which way the plane is facing
		if (Normal.X > 0.0)
		{
			min.X = box.Minimum.X;
			max.X = box.Maximum.X;
		}
		else
		{
			min.X = box.Maximum.X;
			max.X = box.Minimum.X;
		}

		if (Normal.Y > 0.0)
		{
			min.Y = box.Minimum.Y;
			max.Y = box.Maximum.Y;
		}
		else
		{
			min.Y = box.Maximum.Y;
			max.Y = box.Minimum.Y;
		}

		if (Normal.Z > 0.0)
		{
			min.Z = box.Minimum.Z;
			max.Z = box.Maximum.Z;
		}
		else
		{
			min.Z = box.Maximum.Z;
			max.Z = box.Minimum.Z;
		}

		if (IsOnNormalSide(min))
			return PlaneVolumeIntersection::OnNormalSide;
		if (IsOnNormalSide(max))
			return PlaneVolumeIntersection::Intersects;

		return PlaneVolumeIntersection::OnOppositeNormalSide;
	}
}