#include "TransformComponent.h"

namespace Coco
{
	TransformComponent::TransformComponent(SceneEntity* entity) : EntityComponent(entity),
		_position(Vector3::Zero), _rotation(Quaternion::Identity), _scale(Vector3::One), _transformMatrix(Matrix4x4::Identity), _isMatrixDirty(false)
	{}

	void TransformComponent::SetPosition(const Vector3& position)
	{
		_position = position;
		_isMatrixDirty = true;
	}

	void TransformComponent::SetRotation(const Quaternion& rotation)
	{
		_rotation = rotation;
		_isMatrixDirty = true;
	}

	void TransformComponent::SetScale(const Vector3& scale)
	{
		_scale = scale;
		_isMatrixDirty = true;
	}

	void TransformComponent::SetTransformMatrix(const Matrix4x4& matrix)
	{
		_transformMatrix = matrix;
		_isMatrixDirty = false;
	}

	const Matrix4x4& TransformComponent::GetTransformMatrix()
	{
		if (_isMatrixDirty)
			UpdateTransformMatrix();

		return _transformMatrix;
	}

	void TransformComponent::UpdateTransformMatrix()
	{
		_transformMatrix = Matrix4x4::CreateTransform(_position, _rotation, _scale);
		_isMatrixDirty = false;
	}
}
