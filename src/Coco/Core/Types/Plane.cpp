#include "Corepch.h"
#include "Plane.h"

namespace Coco
{
	Plane::Plane() :
		Plane(Vector3::Zero, Vector3::Up)
	{}

	Plane::Plane(const Vector3& origin, const Vector3& normal) :
		Origin(origin),
		Normal(normal.Normalized())
	{}

	Plane::Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2) :
		Origin((p0 + p1 + p2) / 3.0)
	{
		Vector3 a = (p1 - p0).Normalized();
		Vector3 b = (p2 - p1).Normalized();
		Normal = b.Cross(a).Normalized();
	}

	Vector3 Plane::GetClosestPoint(const Vector3& point) const
	{
		Vector3 p = point.Project(Normal);
		return point - p;
	}

	double Plane::GetDistance(const Vector3& point) const
	{
		return Vector3::DistanceBetween(GetClosestPoint(point), point);
	}

	bool Plane::IsOnNormalSide(const Vector3& point) const
	{
		Vector3 p = point.Project(Normal);

		return p.Dot(Normal) > 0;
	}
}