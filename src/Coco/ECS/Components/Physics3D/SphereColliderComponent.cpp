#include "ECSpch.h"
#include "SphereColliderComponent.h"

#include <Coco/Physics3D/CollisionShapes/SphereCollisionShape.h>
#include "../Transform3DComponent.h"

namespace Coco::ECS
{
	SphereColliderComponent::SphereColliderComponent(const Entity& owner, double radius) :
		ColliderComponent(owner),
		_radius(radius)
	{}

	SharedRef<Physics3D::CollisionShape3D> SphereColliderComponent::GetCollisionShape() const
	{
		double radius = _radius;

		Transform3DComponent* transform = nullptr;
		if (GetOwner().TryGetComponent(transform))
		{
			Vector3 scale = transform->GetScale(TransformSpace::Global);

			radius *= Math::Max(scale.X, Math::Max(scale.Y, scale.Z));
		}

		return CreateSharedRef<Physics3D::SphereCollisionShape>(radius);
	}

	void SphereColliderComponent::SetRadius(double radius)
	{
		_radius = Math::Max(0.0, radius);

		// TODO: notify body that collider changed
	}
}