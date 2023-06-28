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
		_localRotation = Quaternion(eulerAngles);
		InvalidateTransform();
	}

	void TransformComponent::SetLocalScale(const Vector3& scale)
	{
		_localScale = scale;
		InvalidateTransform();
	}

	//void TransformComponent::SetGlobalPosition(const Vector3& position)
	//{
	//	// TODO: convert position to local position
	//	// _localPosition = ...
	//	InvalidateTransform();
	//}

	Vector3 TransformComponent::GetGlobalPosition()
	{
		UpdateGlobalTransform();

		return _globalTransformMatrix.GetPosition();
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

		ECSService* ecs = ECSService::Get();
		Entity* parent = nullptr;
		
		if (_inheritParentTransform && ecs->TryGetEntityParent(Owner, parent))
		{
			TransformComponent& parentTransform = ecs->GetComponent<TransformComponent>(parent->GetID());
			_globalTransformMatrix = GetLocalTransformMatrix() * parentTransform.GetGlobalTransformMatrix();
		}
		else
		{
			_globalTransformMatrix = GetLocalTransformMatrix();
		}
		
		_isGlobalTransformMatrixDirty = false;
	}
}