#pragma once

#include "../EntityComponent.h"
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Quaternion.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/List.h>

namespace Coco::ECS
{
	struct COCOAPI TransformComponent : public EntityComponent
	{
	private:
		bool _isLocalTransformMatrixDirty = false;
		Matrix4x4 _localTransformMatrix = Matrix4x4::Identity;

		Vector3 _localPosition = Vector3::Zero;
		Vector3 _localEulerAngles = Vector3::Zero;
		Quaternion _localRotation = Quaternion::Identity;
		Vector3 _localScale = Vector3::One;

		bool _isGlobalTransformMatrixDirty = false;
		Matrix4x4 _globalTransformMatrix = Matrix4x4::Identity;
		Matrix4x4 _invGlobalTransformMatrix = Matrix4x4::Identity;

		bool _inheritParentTransform = true;

	public:
		TransformComponent() = default;
		TransformComponent(EntityID owner);

		void SetLocalTransformMatrix(const Matrix4x4& matrix);
		const Matrix4x4& GetLocalTransformMatrix();

		const Matrix4x4& GetGlobalTransformMatrix();
		const Matrix4x4& GetInverseGlobalTransformMatrix();

		void SetInheritParentTransform(bool inheritParentTransform);
		bool GetInheritParentTransform() const { return _inheritParentTransform; }

		void SetLocalPosition(const Vector3& position);
		const Vector3& GetLocalPosition() const { return _localPosition; }

		void SetLocalRotation(const Quaternion& rotation);
		const Quaternion& GetLocalRotation() const { return _localRotation; }

		void SetLocalEulerAngles(const Vector3& eulerAngles);
		const Vector3& GetLocalEulerAngles() const { return _localEulerAngles; }

		void SetLocalScale(const Vector3& scale);
		const Vector3& GetLocalScale() const { return _localScale; }

		Vector3 GlobalToLocal(const Vector3& vector, bool isPoint = true);
		Vector3 LocalToGlobal(const Vector3& vector, bool isPoint = true);

		Quaternion GlobalToLocal(const Quaternion& rotation);
		Quaternion LocalToGlobal(const Quaternion& rotation);

		void SetGlobalPosition(const Vector3& position);
		Vector3 GetGlobalPosition();

		void SetGlobalRotation(const Quaternion& rotation);
		Quaternion GetGlobalRotation();

		void SetGlobalEulerAngles(const Vector3& eulerAngles);
		Vector3 GetGlobalEulerAngles();

		void SetGlobalScale(const Vector3& scale);
		Vector3 GetGlobalScale();

	private:
		void InvalidateTransform();
		void UpdateLocalTransform();
		void UpdateGlobalTransform();
		bool TryGetParent(TransformComponent*& parentTransform);
	};
}