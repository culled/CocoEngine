#pragma once

#include "../EntityComponent.h"

#include <Coco/Physics3D/CollisionShape3D.h>

namespace Coco::ECS
{
	class ColliderComponent :
		public EntityComponent
	{
	protected:
		ColliderComponent(const Entity& owner);

	public:
		virtual ~ColliderComponent() = default;

		virtual SharedRef<Physics3D::CollisionShape3D> GetCollisionShape() const = 0;
	};
}