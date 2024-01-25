#pragma once

#include <Coco/Core/Types/Refs.h>

#include "Rigidbody3D.h"
#include "PhysicsWorld3D.h"

namespace Coco::Physics3D
{
	class Physics3DPlatform
	{
	public:
		virtual ~Physics3DPlatform() = default;

		virtual Ref<PhysicsWorld3D> CreateWorld() = 0;
		virtual void TryDestroyWorld(Ref<PhysicsWorld3D>& world) = 0;

		virtual Ref<Rigidbody3D> CreateRigidbody(const Rigidbody3DCreateInfo& createInfo) = 0;
		virtual void TryDestroyRigidbody(Ref<Rigidbody3D>& rigidbody) = 0;
	};
}