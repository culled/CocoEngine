#include "TransformComponent.h"
#include "../ECSService.h"
#include "../Entity.h"

namespace Coco::ECS
{
	TransformComponent::TransformComponent(const EntityID& owner) : EntityComponent(owner)
	{}

	TransformComponent::TransformComponent(const EntityID & owner, const Vector3 & localPosition, const Quaternion & localRotation, const Vector3 & localScale) : 
		EntityComponent(owner),
		_localPosition(localPosition), 
		_localRotation(localRotation), 
		_localScale(localScale), 
		_localEulerAngles(localRotation.ToEulerAngles())
	{
		InvalidateTransform();
	}

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
		InvalidateTransform();
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

	bool TransformComponent::TryGetParent(TransformComponent*& parentTransform)
	{
		ECSService* ecs = ECSService::Get();
		Entity* parent = nullptr;

		if (_inheritParentTransform && ecs->TryGetEntityParent(Owner, parent) && ecs->HasComponent<TransformComponent>(parent->ID))
		{
			parentTransform = &ecs->GetComponent<TransformComponent>(parent->ID);
			return true;
		}

		return false;
	}

	void TransformComponent::SetPositionAndRotation(const Vector3& position, const Quaternion& rotation, bool globalSpace)
	{
		if (globalSpace)
		{
			SetGlobalPosition(position);
			SetGlobalRotation(rotation);
		}
		else
		{
			SetLocalPosition(position);
			SetLocalRotation(rotation);
		}
	}

	void TransformComponent::SetPositionAndRotation(const Vector3& position, const Vector3& eulerAngles, bool globalSpace)
	{
		if (globalSpace)
		{
			SetGlobalPosition(position);
			SetGlobalEulerAngles(eulerAngles);
		}
		else
		{
			SetLocalPosition(position);
			SetLocalEulerAngles(eulerAngles);
		}
	}

	void TransformComponent::SetTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool globalSpace)
	{
		if (globalSpace)
		{
			SetGlobalPosition(position);
			SetGlobalRotation(rotation);
			SetGlobalScale(scale);
		}
		else
		{
			SetLocalPosition(position);
			SetLocalRotation(rotation);
			SetLocalScale(scale);
		}
	}

	void TransformComponent::SetTransform(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale, bool globalSpace)
	{
		if (globalSpace)
		{
			SetGlobalPosition(position);
			SetGlobalEulerAngles(eulerAngles);
			SetGlobalScale(scale);
		}
		else
		{
			SetLocalPosition(position);
			SetLocalEulerAngles(eulerAngles);
			SetLocalScale(scale);
		}
	}

	void TransformComponent::InvalidateTransform()
	{
		ECSService* ecs = ECSService::Get();
		const auto childrenIDs = ecs->GetEntityChildrenIDs(Owner);
		
		for (const auto& childID : childrenIDs)
		{
			if(ecs->HasComponent<TransformComponent>(childID))
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
		if (_inheritParentTransform && TryGetParent(parentTransform))
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
}