#pragma once

#include <Coco/Core/Types/Transform.h>
#include "Rigidbody3DTypes.h"

namespace Coco::Physics3D
{
	class Rigidbody3D
	{
	public:
		virtual ~Rigidbody3D() = default;

		virtual void SetMass(double mass) = 0;
		virtual double GetMass() const = 0;

		virtual void SetType(Rigidbody3DType type) = 0;
		virtual Rigidbody3DType GetType() const = 0;

		virtual void SetTransform(const Vector3& position, const Quaternion& rotation) = 0;
		virtual void GetTransform(Vector3& outPosition, Quaternion& outRotation, bool withInterpolation) const = 0;

		virtual bool IsAwake() const = 0;

		virtual Vector3 GetLinearVelocity() const = 0;
		virtual Vector3 GetAngularVelocity() const = 0;
	};
}