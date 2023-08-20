#include "RectTransformComponent.h"
#include <Coco/ECS/ECSService.h>

namespace Coco::ECS
{
	RectTransformComponent::RectTransformComponent(const EntityID& owner) : EntityComponent(owner),
		_localPosition(Vector2::Zero),
		_localRotation(0),
		_localSize(Size(100, 100)),
		_localScale(Vector2::One),
		_pivot(Vector2::Zero),
		_inheritParentTransform(true),
		_localZIndex(0)
	{}

	RectTransformComponent::RectTransformComponent(const EntityID& owner, const Vector2& position, double rotation, const Size& size, const Vector2& pivot, int zIndex) :
		EntityComponent(owner),
		_localPosition(position),
		_localRotation(rotation),
		_localSize(size),
		_localScale(Vector2::One),
		_pivot(pivot),
		_localZIndex(zIndex)
	{
		InvalidateTransform();
	}

	const Matrix4x4& RectTransformComponent::GetLocalTransformMatrix()
	{
		UpdateLocalTransform();

		return _localTransformMatrix;
	}

	Matrix4x4 RectTransformComponent::GetGlobalTransformMatrix(bool sized)
	{
		UpdateGlobalTransform();
	
		if(sized)
			return _globalTransformMatrix;

		// For an unsized matrix, just remove the size component from the scale
		const double invSizeW = 1.0 / _localSize.Width;
		const double invSizeH = 1.0 / _localSize.Height;

		Matrix4x4 m = _globalTransformMatrix;
		m.Data[Matrix4x4::m11] *= invSizeW;
		m.Data[Matrix4x4::m21] *= invSizeW;
		m.Data[Matrix4x4::m31] *= invSizeW;

		m.Data[Matrix4x4::m13] *= invSizeH;
		m.Data[Matrix4x4::m23] *= invSizeH;
		m.Data[Matrix4x4::m33] *= invSizeH;
		return m;
	}

	const Matrix4x4& RectTransformComponent::GetInverseGlobalTransformMatrix()
	{
		UpdateGlobalTransform();

		return _invGlobalTransformMatrix;
	}

	void RectTransformComponent::SetInheritParentTransform(bool inheritParentTransform)
	{
		_inheritParentTransform = inheritParentTransform;
		InvalidateTransform();
	}

	void RectTransformComponent::SetLocalPosition(const Vector2& position)
	{
		_localPosition = position;
		InvalidateTransform();
	}

	void RectTransformComponent::SetLocalRotation(double rotation)
	{
		_localRotation = rotation;
		InvalidateTransform();
	}

	void RectTransformComponent::SetPivot(const Vector2& pivot)
	{
		_pivot = pivot;
		InvalidateTransform();
	}

	void RectTransformComponent::SetLocalSize(const Size& size)
	{
		_localSize = size;
		InvalidateTransform();
	}

	void RectTransformComponent::SetLocalZIndex(int zIndex)
	{
		_localZIndex = zIndex;
		InvalidateTransform();
	}

	void RectTransformComponent::SetLocalScale(const Vector2& scale)
	{
		_localScale = scale;
		InvalidateTransform();
	}

	bool RectTransformComponent::TryGetParent(RectTransformComponent*& parentTransform)
	{
		ECSService* ecs = ECSService::Get();
		Entity* parent = nullptr;

		if (_inheritParentTransform && ecs->TryGetEntityParent(Owner, parent) && ecs->HasComponent<RectTransformComponent>(parent->ID))
		{
			parentTransform = &ecs->GetComponent<RectTransformComponent>(parent->ID);
			return true;
		}

		return false;
	}

	void RectTransformComponent::InvalidateTransform()
	{
		ECSService* ecs = ECSService::Get();
		const auto childrenIDs = ecs->GetEntityChildrenIDs(Owner);

		for (const auto& childID : childrenIDs)
		{
			if(ecs->HasComponent<RectTransformComponent>(childID))
				ecs->GetComponent<RectTransformComponent>(childID).InvalidateTransform();
		}

		_isLocalTransformMatrixDirty = true;
		_isGlobalTransformMatrixDirty = true;
	}

	void RectTransformComponent::UpdateLocalTransform()
	{
		if (!_isLocalTransformMatrixDirty)
			return;

		Quaternion rotation(Vector3::Forwards, _localRotation);
		Vector3 scale(_localSize.Width * _localScale.X, 1.0, _localSize.Height * _localScale.Y);
		Vector3 pivotOffset = rotation * (Vector3(_pivot.X, 0.0, _pivot.Y) * scale);

		_localTransformMatrix = Matrix4x4::CreateTransform(Vector3(_localPosition.X, _localZIndex, _localPosition.Y) - pivotOffset, rotation, scale);
		_isLocalTransformMatrixDirty = false;
	}

	void RectTransformComponent::UpdateGlobalTransform()
	{
		if (!_isGlobalTransformMatrixDirty)
			return;

		RectTransformComponent* parentTransform;
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
