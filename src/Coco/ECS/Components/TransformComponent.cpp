#include "TransformComponent.h"
#include "../ECSService.h"
#include "../Entity.h"

namespace Coco::ECS
{
	TransformComponent::TransformComponent(EntityID owner) : EntityComponent(owner)
	{}

	void TransformComponent::SetLocalTransformMatrix(const Matrix4x4& matrix)
	{
		_localTransformMatrix = matrix;
		_isLocalTransformMatrixDirty = true;
	}

	const Matrix4x4& TransformComponent::GetLocalTransformMatrix()
	{
		UpdateLocalTransform();

		return _localTransformMatrix;
	}

	const Matrix4x4& TransformComponent::GetGlobalTransformMatrix()
	{
		UpdateGlobalTransform();

		return _globalTransformMatrix;
	}

	const Matrix4x4& TransformComponent::GetInverseGlobalTransformMatrix()
	{
		UpdateGlobalTransform();

		return _invGlobalTransformMatrix;
	}

	void TransformComponent::SetInheritParentTransform(bool inheritParentTransform)
	{
		_inheritParentTransform = inheritParentTransform;
	}

	void TransformComponent::SetLocalPosition(const Vector3& position)
	{
		_localPosition = position;
		InvalidateTransform();
	}

	void TransformComponent::SetLocalRotation(const Quaternion& rotation)
	{
		_localRotation = rotation;
		_localEulerAngles = rotation.ToEulerAngles();
		InvalidateTransform();
	}

	void TransformComponent::SetLocalEulerAngles(const Vector3& eulerAngles)
	{
		_localEulerAngles = eulerAngles;
		_localRotation = Quaternion(_localEulerAngles);
		InvalidateTransform();
	}

	void TransformComponent::SetLocalScale(const Vector3& scale)
	{
		_localScale = scale;
		InvalidateTransform();
	}

	Vector3 TransformComponent::GlobalToLocal(const Vector3& vector, bool isPoint)
	{
		UpdateGlobalTransform();

		return Vector3(_invGlobalTransformMatrix * Vector4(vector, isPoint ? 1.0 : 0.0));
	}

	Vector3 TransformComponent::LocalToGlobal(const Vector3& vector, bool isPoint)
	{
		UpdateGlobalTransform();

		return Vector3(_globalTransformMatrix * Vector4(vector, isPoint ? 1.0 : 0.0));
	}

	Quaternion TransformComponent::GlobalToLocal(const Quaternion& rotation)
	{
		UpdateGlobalTransform();

		return _invGlobalTransformMatrix * rotation;
	}

	Quaternion TransformComponent::LocalToGlobal(const Quaternion& rotation)
	{
		UpdateGlobalTransform();

		return _globalTransformMatrix * rotation;
	}

	void TransformComponent::SetGlobalPosition(const Vector3& position)
	{
		TransformComponent* parent;
		if (TryGetParent(parent))
			SetLocalPosition(parent->GlobalToLocal(position));
		else
			SetLocalPosition(position);
	}

	Vector3 TransformComponent::GetGlobalPosition()
	{
		TransformComponent* parent;
		if (TryGetParent(parent))
			return parent->LocalToGlobal(_localPosition);
		else
			return _localPosition;
	}

	void TransformComponent::SetGlobalRotation(const Quaternion& rotation)
	{
		TransformComponent* parent;
		if (TryGetParent(parent))
			SetLocalRotation(parent->GlobalToLocal(rotation));
		else
			SetLocalRotation(rotation);
	}

	Quaternion TransformComponent::GetGlobalRotation()
	{
		TransformComponent* parent;
		if (TryGetParent(parent))
			return parent->LocalToGlobal(_localRotation);
		else
			return _localRotation;
	}

	void TransformComponent::SetGlobalEulerAngles(const Vector3& eulerAngles)
	{
		SetGlobalRotation(Quaternion(eulerAngles));
	}

	Vector3 TransformComponent::GetGlobalEulerAngles()
	{
		return GetGlobalRotation().ToEulerAngles();
	}

	void TransformComponent::SetGlobalScale(const Vector3& scale)
	{
		TransformComponent* parent;
		if (TryGetParent(parent))
			SetLocalScale(parent->GlobalToLocal(scale, false));
		else
			SetLocalScale(scale);
	}

	Vector3 TransformComponent::GetGlobalScale()
	{
		TransformComponent* parent;
		if (TryGetParent(parent))
			return parent->LocalToGlobal(_localScale, false);
		else
			return _localScale;
	}

	void TransformComponent::InvalidateTransform()
	{
		ECSService* ecs = ECSService::Get();
		const auto childrenIDs = ecs->GetEntityChildrenIDs(Owner);
		
		for (const auto& childID : childrenIDs)
		{
			ecs->GetComponent<TransformComponent>(childID).InvalidateTransform();
		}
		
		_isLocalTransformMatrixDirty = true;
		_isGlobalTransformMatrixDirty = true;
	}

	void TransformComponent::UpdateLocalTransform()
	{
		if (!_isLocalTransformMatrixDirty)
			return;

		_localTransformMatrix = Matrix4x4::CreateTransform(_localPosition, _localRotation, _localScale);
		_isLocalTransformMatrixDirty = false;
	}

	void TransformComponent::UpdateGlobalTransform()
	{
		if (!_isGlobalTransformMatrixDirty)
			return;
		
		TransformComponent* parentTransform;
		if (TryGetParent(parentTransform))
		{
			_globalTransformMatrix = GetLocalTransformMatrix() * parentTransform->GetGlobalTransformMatrix();
		}
		else
		{
			_globalTransformMatrix = GetLocalTransformMatrix();
		}

		_invGlobalTransformMatrix = _globalTransformMatrix.Inverted();
		
		_isGlobalTransformMatrixDirty = false;
	}

	bool TransformComponent::TryGetParent(TransformComponent*& parentTransform)
	{
		ECSService* ecs = ECSService::Get();
		Entity* parent = nullptr;

		if (_inheritParentTransform && ecs->TryGetEntityParent(Owner, parent))
		{
			parentTransform = &ecs->GetComponent<TransformComponent>(parent->GetID());
			return true;
		}

		return false;
	}
}