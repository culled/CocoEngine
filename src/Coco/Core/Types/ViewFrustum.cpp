#include "Corepch.h"
#include "ViewFrustum.h"
#include "BoundingBox.h"

namespace Coco
{
	ViewFrustum::ViewFrustum() :
		Near(),
		Far(),
		Top(),
		Bottom(),
		Right(),
		Left()
	{}

	ViewFrustum::ViewFrustum(
		const Vector3& origin,
		const Vector3& direction,
		const Vector3& up,
		const Vector3& nearOffset,
		const Size& nearSize,
		const Vector3& farOffset,
		const Size& farSize)
	{
		Vector3 right = direction.Cross(up);

		Vector3 nearCenter = origin + direction * nearOffset.Z + up * nearOffset.Y + right * nearOffset.X;
		Vector3 nearBottom = nearCenter - up * nearSize.Height * 0.5;
		Vector3 nearTop = nearBottom + up * nearSize.Height;
		Vector3 nearLeft = nearCenter - right * nearSize.Width * 0.5;
		Vector3 nearRight = nearLeft + right * nearSize.Width;

		Vector3 farCenter = origin + direction * farOffset.Z + up * farOffset.Y + right * farOffset.X;
		Vector3 farBottom = farCenter - up * farSize.Height * 0.5;
		Vector3 farTop = farBottom + up * farSize.Height;
		Vector3 farLeft = farCenter - right * farSize.Width * 0.5;
		Vector3 farRight = farLeft + right * farSize.Width;

		Near = Plane(nearCenter, direction);
		Far = Plane(farCenter, -direction);

		Top = Plane((nearTop + farTop) / 2.0, right.Cross(nearTop - farTop).Normalized());
		Bottom = Plane((nearBottom + farBottom) / 2.0, right.Cross(farBottom - nearBottom).Normalized());

		Right = Plane((nearRight + farRight) / 2.0, up.Cross(farRight - nearRight).Normalized());
		Left = Plane((nearLeft + farLeft) / 2.0, up.Cross(nearLeft - farLeft).Normalized());
	}

	ViewFrustum ViewFrustum::CreatePerspective(
		const Vector3& origin, 
		const Vector3& direction,
		const Vector3& up,
		double verticalFOVRadians, 
		double aspectRatio, 
		double nearClip, 
		double farClip)
	{
		double vT = Math::Tan(verticalFOVRadians * 0.5);
		double nearHeight = vT * nearClip * 2.0;
		double farHeight = vT * farClip * 2.0;

		return ViewFrustum(
			origin,
			direction,
			up,
			Vector3(0.0, 0.0, nearClip),
			Size(nearHeight * aspectRatio, nearHeight),
			Vector3(0.0, 0.0, farClip),
			Size(farHeight * aspectRatio, farHeight));
	}

	ViewFrustum ViewFrustum::CreateOrthographic(
		const Vector3& origin, 
		const Vector3& direction, 
		const Vector3& up, 
		double size, 
		double aspectRatio, 
		double nearClip, 
		double farClip)
	{
		Size s(size * aspectRatio, size);
		return ViewFrustum(origin,
			direction,
			up,
			Vector3(0, 0, nearClip),
			s,
			Vector3(0, 0, farClip),
			s);
	}

	ViewFrustum ViewFrustum::CreateOrthographic(
		const Vector3& origin, 
		const Vector3& direction, 
		const Vector3& up, 
		double left, 
		double right, 
		double bottom, 
		double top, 
		double nearClip, 
		double farClip)
	{ 
		return ViewFrustum(origin,
			direction,
			up,
			Vector3(right - left, top - bottom, nearClip),
			Size(right - left, top - bottom),
			Vector3(right - left, top - bottom, farClip),
			Size(right - left, top - bottom)
		);
	}

	bool ViewFrustum::IsInside(const Vector3& point) const
	{
		return Near.IsOnNormalSide(point) &&
			Far.IsOnNormalSide(point) &&
			Right.IsOnNormalSide(point) &&
			Left.IsOnNormalSide(point) &&
			Top.IsOnNormalSide(point) &&
			Bottom.IsOnNormalSide(point);
	}

	bool ViewFrustum::IsInside(const BoundingBox& bounds) const
	{
		return Near.CheckIntersection(bounds) != PlaneVolumeIntersection::OnOppositeNormalSide &&
			Far.CheckIntersection(bounds) != PlaneVolumeIntersection::OnOppositeNormalSide &&
			Top.CheckIntersection(bounds) != PlaneVolumeIntersection::OnOppositeNormalSide &&
			Bottom.CheckIntersection(bounds) != PlaneVolumeIntersection::OnOppositeNormalSide &&
			Left.CheckIntersection(bounds) != PlaneVolumeIntersection::OnOppositeNormalSide &&
			Right.CheckIntersection(bounds) != PlaneVolumeIntersection::OnOppositeNormalSide;
	}
}