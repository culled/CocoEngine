#pragma once

#include <Coco/Core/Types/Vector.h>

#include "Rigidbody3D.h"

namespace Coco::Physics3D
{
	class PhysicsWorld3D
	{
	public:
		virtual ~PhysicsWorld3D() = default;

		virtual void Simulate(double deltaTime) = 0;

		virtual void SetGravity(const Vector3& gravity) = 0;
		virtual Vector3 GetGravity() const = 0;

		virtual void SetFixedTimestep(double deltaTime) = 0;
		virtual double GetFixedTimestep() const = 0;

		virtual void AddRigidbody(Ref<Rigidbody3D> rigidbody) = 0;
		virtual void RemoveRigidbody(Ref<Rigidbody3D> rigidbody) = 0;
	};
}