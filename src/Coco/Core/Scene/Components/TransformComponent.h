#pragma once

#include "EntityComponent.h"
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Quaternion.h>
#include <Coco/Core/Types/Matrix.h>

namespace Coco
{
	class COCOAPI TransformComponent : public EntityComponent
	{
	private:
		Vector3 _position;
		Quaternion _rotation;
		Vector3 _scale;
		bool _isMatrixDirty;
		Matrix4x4 _transformMatrix;

	public:
		TransformComponent(SceneEntity* entity);

		void SetPosition(const Vector3& position);
		Vector3 GetPosition() const { return _position; }

		void SetRotation(const Quaternion& rotation);
		Quaternion GetRotation() const { return _rotation; }

		void SetScale(const Vector3& scale);
		Vector3 GetScale() const { return _scale; }

		void SetTransformMatrix(const Matrix4x4& matrix);
		const Matrix4x4& GetTransformMatrix();

	private:
		void UpdateTransformMatrix();
	};
}