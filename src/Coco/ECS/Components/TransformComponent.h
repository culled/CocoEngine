#pragma once

#include "../EntityComponent.h"
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Types/Quaternion.h>
#include <Coco/Core/Types/Matrix.h>
#include <Coco/Core/Types/List.h>

namespace Coco::ECS
{
	/// @brief Defines 3D transform data for an entity
	class COCOAPI TransformComponent : public EntityComponent
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
		TransformComponent(const EntityID& owner);
		TransformComponent(const EntityID& owner, const Vector3& localPosition, const Quaternion& localRotation, const Vector3& localScale);

		/// @brief Sets the transform matrix local to the parent
		/// @param matrix The local transform matrix
		void SetLocalTransformMatrix(const Matrix4x4& matrix);

		/// @brief Gets the transform matrix local to the parent
		/// @return The local transform matrix
		const Matrix4x4& GetLocalTransformMatrix();

		/// @brief Gets the global transform matrix
		/// @return The global transform matrix
		const Matrix4x4& GetGlobalTransformMatrix();

		/// @brief Gets the inverse of the global transform matrix
		/// @return The inverse of the global transform matrix
		const Matrix4x4& GetInverseGlobalTransformMatrix();

		/// @brief Sets if this transform should inherit its parent's transform
		/// @param inheritParentTransform If true, this entity's transform will move with its parent's
		void SetInheritParentTransform(bool inheritParentTransform);

		/// @brief Gets if this transform inherits and moves with its parent's transform
		/// @return If this transform inherits and moves with its parent's transform
		bool GetInheritParentTransform() const { return _inheritParentTransform; }

		/// @brief Sets the position local to the parent
		/// @param position The local position
		void SetLocalPosition(const Vector3& position);

		/// @brief Gets the position local to the parent
		/// @return The local position
		const Vector3& GetLocalPosition() const { return _localPosition; }

		/// @brief Sets the rotation local to the parent
		/// @param rotation The local rotation
		void SetLocalRotation(const Quaternion& rotation);

		/// @brief Gets the rotation local to the parent
		/// @return The local rotation
		const Quaternion& GetLocalRotation() const { return _localRotation; }

		/// @brief Sets the rotation local to the parent in euler angles.
		/// NOTE: this will be converted to a Quaternion internally,
		/// so GetLocalEulerAngles() may not return the same angles that this function is given
		/// @param eulerAngles The local euler angles
		void SetLocalEulerAngles(const Vector3& eulerAngles);

		/// @brief Gets the rotation local to the parent in euler angles.
		/// NOTE: this is converted from a Quaternion internally,
		/// so this may return different angles than those given to SetLocalEulerAngles()
		/// @return The local euler angles
		const Vector3& GetLocalEulerAngles() const { return _localEulerAngles; }

		/// @brief Sets the scale local to the parent
		/// @param scale The local scale
		void SetLocalScale(const Vector3& scale);

		/// @brief Gets the scale local to the parent
		/// @return The local scale
		const Vector3& GetLocalScale() const { return _localScale; }

		/// @brief Converts a vector from global space to local space (local to this transform)
		/// @param vector The vector
		/// @param isPoint If true, then the vector will be interpreted as a position instead of a direction
		/// @return The vector in space local to this transform
		Vector3 GlobalToLocal(const Vector3& vector, bool isPoint = true);

		/// @brief Converts a vector from local space (local to this transform) to global space
		/// @param vector The vector
		/// @param isPoint If true, then the vector will be interpreted as a position instead of a direction
		/// @return The vector in global space
		Vector3 LocalToGlobal(const Vector3& vector, bool isPoint = true);

		/// @brief Converts a rotation from global space to local space (local to this transform)
		/// @param rotation The rotation
		/// @return The rotation in space local to this transform
		Quaternion GlobalToLocal(const Quaternion& rotation);

		/// @brief Converts a rotation from local space (local to this transform) to global space
		/// @param rotation The rotation
		/// @return The rotation in global space
		Quaternion LocalToGlobal(const Quaternion& rotation);

		/// @brief Sets the position of this transform from a position in global space
		/// @param position The global position
		void SetGlobalPosition(const Vector3& position);

		/// @brief Gets the position of this transform in global space
		/// @return The global space
		Vector3 GetGlobalPosition();

		/// @brief Sets the rotation of this transform from a rotation in global space
		/// @param rotation The global rotation
		void SetGlobalRotation(const Quaternion& rotation);

		/// @brief Gets the rotation of this transform in global space
		/// @return The global rotation
		Quaternion GetGlobalRotation();

		/// @brief Sets the rotation of this transform from a euler rotation in global space.
		/// NOTE: this will be converted to a Quaternion internally,
		/// so GetGlobalEulerAngles() may not return the same angles that this function is given
		/// @param eulerAngles The global euler angles
		void SetGlobalEulerAngles(const Vector3& eulerAngles);

		/// @brief Gets the rotation of this transform as euler angles in global space
		/// NOTE: this is converted from a Quaternion internally,
		/// so this may return different angles than those given to SetGlobalEulerAngles()
		/// @return The global euler angles
		Vector3 GetGlobalEulerAngles();

		/// @brief Sets the scale of this transform in global space
		/// @param scale The global scale
		void SetGlobalScale(const Vector3& scale);

		/// @brief Gets the scale of this transform in global space
		/// @return The global scale
		Vector3 GetGlobalScale();

		/// @brief Gets the parent transform, checking first if it exists
		/// @param parentTransform Will be set to the parent transform
		/// @return True if the parent transform exists
		bool TryGetParent(TransformComponent*& parentTransform);

		/// @brief Sets the position and rotation of this transform
		/// @param position The position
		/// @param rotation The rotation
		/// @param globalSpace If true, the position and rotation will be interpreted in global space instead of local space
		void SetPositionAndRotation(const Vector3& position, const Quaternion& rotation, bool globalSpace = false);

		/// @brief Sets the position and rotation of this transform
		/// @param position The position
		/// @param eulerAngles The euler angles
		/// @param globalSpace If true, the position and rotation will be interpreted in global space instead of local space
		void SetPositionAndRotation(const Vector3& position, const Vector3& eulerAngles, bool globalSpace = false);

		/// @brief Sets the position, rotation, and scale of this transform
		/// @param position The position
		/// @param rotation The rotation
		/// @param scale The scale
		/// @param globalSpace If true, the position, rotation, and scale will be interpreted in global space instead of local space
		void SetTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale, bool globalSpace = false);

		/// @brief Sets the position, rotation, and scale of this transform
		/// @param position The position
		/// @param eulerAngles The euler angles
		/// @param scale The scale
		/// @param globalSpace If true, the position, rotation, and scale will be interpreted in global space instead of local space
		void SetTransform(const Vector3& position, const Vector3& eulerAngles, const Vector3& scale, bool globalSpace = false);

	private:
		/// @brief Marks this transform as needing to be recalculated
		void InvalidateTransform();

		/// @brief Updates the local transform matrix
		void UpdateLocalTransform();

		/// @brief Updates the global transform matrix
		void UpdateGlobalTransform();
	};
}