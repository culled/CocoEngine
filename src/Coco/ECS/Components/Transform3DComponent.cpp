#include "ECSpch.h"
#include "Transform3DComponent.h"

namespace Coco::ECS
{
	Transform3DComponent::Transform3DComponent() :
		Transform3DComponent(Transform3D(), true)
	{
	}

	Transform3DComponent::Transform3DComponent(const Transform3D& transform, bool inheritParentTransform) :
		Transform(transform),
		InheritParentTransform(inheritParentTransform)
	{
	}

	Transform3DComponent::Transform3DComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool inheritParentTransform) :
		Transform3DComponent(Transform3D(position, rotation, scale), inheritParentTransform)
	{}
}