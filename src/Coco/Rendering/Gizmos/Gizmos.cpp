#include "Renderpch.h"
#include "Gizmos.h"

#include <Coco/Core/Types/Quaternion.h>

namespace Coco::Rendering
{
	GizmoDrawCall::GizmoDrawCall(uint32 submeshID, const Coco::Color& color, const Matrix4x4& transform) :
		SubmeshID(submeshID),
		Color(color),
		Transform(transform)
	{}

	const uint32 Gizmos::_lineSubmeshID = 0;
	const uint32 Gizmos::_boxSubmeshID = 1;
	const uint32 Gizmos::_sphereSubmeshID = 2;
	std::vector<GizmoDrawCall> Gizmos::_drawCalls;

	void Gizmos::DrawLine3D(const Vector3& start, const Vector3& end, const Color& color)
	{
		Vector3 dir = end - start;
		Matrix4x4 t = Matrix4x4::CreateTransform(start, Quaternion::FromToRotation(Vector3::Forward, dir.Normalized()), Vector3::One * dir.GetLength());
		_drawCalls.emplace_back(_lineSubmeshID, color, t);
	}

	void Gizmos::DrawRay3D(const Vector3& origin, const Vector3& direction, const Color& color)
	{
		DrawLine3D(origin, origin + direction, color);
	}

	void Gizmos::DrawWireBox(const Vector3& origin, const Quaternion& rotation, const Vector3& size, const Color& color)
	{
		DrawWireBox(Matrix4x4::CreateTransform(origin, rotation, size), color);
	}

	void Gizmos::DrawWireBox(const Matrix4x4& transform, const Color& color)
	{
		_drawCalls.emplace_back(_boxSubmeshID, color, transform);
	}

	void Gizmos::DrawWireBounds(const BoundingBox& localBounds, const Matrix4x4& transform, const Color& color)
	{
		BoundingBox b = localBounds.Transformed(transform);
		DrawWireBox(b.GetCenter(), Quaternion::Identity, b.GetSize(), color);
	}

	void Gizmos::DrawWireSphere(double radius, const Vector3& position, const Color& color)
	{
		_drawCalls.emplace_back(_sphereSubmeshID, color, Matrix4x4::CreateTransform(position, Quaternion::Identity, Vector3::One * radius));
	}

	void Gizmos::DrawFrustum(const ViewFrustum& frustum, const Color& color)
	{
		// Draw the near plane
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NTL], frustum.CornerPoints[ViewFrustum::NTR], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NTR], frustum.CornerPoints[ViewFrustum::NBR], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NBR], frustum.CornerPoints[ViewFrustum::NBL], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NBL], frustum.CornerPoints[ViewFrustum::NTL], color);

		// Draw the far plane
		DrawLine3D(frustum.CornerPoints[ViewFrustum::FTL], frustum.CornerPoints[ViewFrustum::FTR], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::FTR], frustum.CornerPoints[ViewFrustum::FBR], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::FBR], frustum.CornerPoints[ViewFrustum::FBL], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::FBL], frustum.CornerPoints[ViewFrustum::FTL], color);

		// Connect the planes
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NTL], frustum.CornerPoints[ViewFrustum::FTL], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NTR], frustum.CornerPoints[ViewFrustum::FTR], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NBL], frustum.CornerPoints[ViewFrustum::FBL], color);
		DrawLine3D(frustum.CornerPoints[ViewFrustum::NBR], frustum.CornerPoints[ViewFrustum::FBR], color);
	}
}