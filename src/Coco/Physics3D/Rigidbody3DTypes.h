#pragma once

#include <Coco/Core/Types/Refs.h>
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Quaternion.h>
#include "CollisionShape3D.h"

namespace Coco::Physics3D
{
	enum class Rigidbody3DType
	{
		Dynamic,
		Kinematic,
		Static
	};

	struct Rigidbody3DCreateInfo
	{
		Vector3 Position;
		Quaternion Rotation;

		// TODO: multiple collision shapes
		SharedRef<CollisionShape3D> CollisionShape;

		double Mass;

		Rigidbody3DType Type;

		Rigidbody3DCreateInfo(
			const Vector3& position,
			const Quaternion& rotation,
			SharedRef<CollisionShape3D> collisionShape,
			double mass = 10.0,
			Rigidbody3DType type = Rigidbody3DType::Dynamic);
	};
}