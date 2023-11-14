#include "ECSpch.h"
#include "Transform3DComponent.h"

namespace Coco::ECS
{
	Transform3DComponent::Transform3DComponent(const Entity& owner) :
		Transform3DComponent(owner, Transform3D(), true)
	{}

	Transform3DComponent::Transform3DComponent(const Entity& owner, const Transform3D& transform, bool inheritParentTransform) :
		EntityComponent(owner),
		_transform(transform),
		_inheritParentTransform(inheritParentTransform)
	{
		Recalculate();
	}

	Transform3DComponent::Transform3DComponent(const Entity& owner, const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool inheritParentTransform) :
		Transform3DComponent(owner, Transform3D(position, rotation, scale), inheritParentTransform)
	{}

	void Transform3DComponent::SetPosition(const Vector3& position, TransformSpace space)
	{
		_transform.SetPosition(position, space);

		Recalculate();
	}

	void Transform3DComponent::SetRotation(const Quaternion& rotation, TransformSpace space)
	{
		_transform.SetRotation(rotation, space);

		Recalculate();
	}

	void Transform3DComponent::SetScale(const Vector3& scale, TransformSpace space)
	{
		_transform.SetScale(scale, space);

		Recalculate();
	}

	void Transform3DComponent::Translate(const Vector3& translation, TransformSpace space)
	{
		_transform.Translate(translation, space);

		Recalculate();
	}

	void Transform3DComponent::Rotate(const Quaternion& rotation, TransformSpace space)
	{
		_transform.Rotate(rotation, space);

		Recalculate();
	}

	void Transform3DComponent::SetInheritParentTransform(bool inheritParentTransform, bool maintainGlobalTransform)
	{
		if (inheritParentTransform == _inheritParentTransform)
			return;

		_inheritParentTransform = inheritParentTransform;

		if (maintainGlobalTransform)
		{
			Vector3 p, s;
			Quaternion r;
			Decompose(TransformSpace::Global, p, r, s);

			const Transform3D* parent = nullptr;
			if (TryGetParentTransform(parent))
			{
				parent->Transform(TransformSpace::Global, TransformSpace::Self, p, r, s);
			}

			_transform.SetPosition(p, TransformSpace::Parent);
			_transform.SetRotation(r, TransformSpace::Parent);
			_transform.SetScale(s, TransformSpace::Parent);
		}

		Recalculate();
	}

	void Transform3DComponent::Recalculate()
	{
		const Transform3D* parentTransform = nullptr;
		TryGetParentTransform(parentTransform);
		Recalculate(parentTransform);
	}

	void Transform3DComponent::Recalculate(const Transform3D* parent)
	{
		_transform.Recalculate(_inheritParentTransform ? parent : nullptr);

		if (!_inheritParentTransform)
			return;

		const Entity& entity = GetOwner();
		for (const Entity& child : entity.GetChildren())
		{
			if (!child.HasComponent<Transform3DComponent>())
				continue;

			child.GetComponent<Transform3DComponent>().Recalculate(&_transform);
		}
	}

	bool Transform3DComponent::TryGetParentTransform(const Transform3D*& outParentTransform) const
	{
		if (!_inheritParentTransform)
			return false;

		const Entity& owner = GetOwner();
		const Transform3DComponent* parentTransform = nullptr;

		if (owner.HasParent() && owner.GetParent().TryGetComponent(parentTransform))
		{
			outParentTransform = &parentTransform->_transform;
			return true;
		}

		return false;
	}
}