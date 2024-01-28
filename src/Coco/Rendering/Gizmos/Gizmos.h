#pragma once

#include <Coco/Core/Defines.h>
#include <Coco/Core/Types/Color.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/ViewFrustum.h>
#include <Coco/Core/Types/BoundingBox.h>

namespace Coco::Rendering
{
	/// @brief A draw call for a gizmo shape
	struct GizmoDrawCall
	{
		uint32 SubmeshID;

		/// @brief The color of this shape
		Color Color;

		/// @brief The transform of this shape
		Matrix4x4 Transform;

		GizmoDrawCall(
			uint32 submeshID,
			const Coco::Color& color,
			const Matrix4x4& transform);
	};

	class Gizmos
	{
		friend class GizmoRenderPass;

	public:
		static void DrawLine3D(const Vector3& start, const Vector3& end, const Color& color);
		static void DrawRay3D(const Vector3& origin, const Vector3& direction, const Color& color);
		static void DrawWireBox(const Vector3& origin, const Quaternion& rotation, const Vector3& size, const Color& color);
		static void DrawWireBox(const Matrix4x4& transform, const Color& color);
		static void DrawWireBounds(const BoundingBox& localBounds, const Matrix4x4& transform, const Color& color);
		static void DrawWireSphere(double radius, const Vector3& position, const Color& color);
		static void DrawFrustum(const ViewFrustum& frustum, const Color& color);

	private:
		static const uint32 _lineSubmeshID;
		static const uint32 _boxSubmeshID;
		static const uint32 _sphereSubmeshID;

		static std::vector<GizmoDrawCall> _drawCalls;
	};
}