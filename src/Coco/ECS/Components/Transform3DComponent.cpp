#include "ECSpch.h"
#include "Transform3DComponent.h"

namespace Coco::ECS
{
	Transform3DComponent::Transform3DComponent() :
		Transform3DComponent(Transform3D())
	{
	}

	Transform3DComponent::Transform3DComponent(const Transform3D& transform) :
		Transform(transform)
	{
	}

	Transform3DComponent::Transform3DComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale) :
		Transform3DComponent(Transform3D(position, rotation, scale))
	{}
}