#include "ECSpch.h"
#include "BoxColliderComponent.h"

#include <Coco/Physics3D/CollisionShapes/BoxCollisionShape.h>
#include "../Transform3DComponent.h"

namespace Coco::ECS
{
	BoxColliderComponent::BoxColliderComponent(const Entity& owner, const Vector3& size) :
		ColliderComponent(owner),
		_size(size)
	{}

	SharedRef<Physics3D::CollisionShape3D> BoxColliderComponent::GetCollisionShape() const
	{
		Vector3 size = _size;

		Transform3DComponent* transform = nullptr;
		if (GetOwner().TryGetComponent(transform))
		{
			size *= transform->GetScale(TransformSpace::Global);
		}

		return CreateSharedRef<Physics3D::BoxCollisionShape>(size);
	}

	void BoxColliderComponent::SetSize(const Vector3& size)
	{
		_size = size;

		_size.X = Math::Max(_size.X, 0.0);
		_size.Y = Math::Max(_size.Y, 0.0);
		_size.Z = Math::Max(_size.Z, 0.0);

		// TODO: notify body that collider changed
	}
}