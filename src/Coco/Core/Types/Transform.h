#pragma once

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace Coco
{
	/// @brief Represents a 2D transformation
	struct Transform2D
	{
		/// @brief The local position
		Vector2 LocalPosition;

		/// @brief The local rotation
		double LocalRotation;

		/// @brief The local scale
		Vector2 LocalScale;

		/// @brief The local transform matrix
		Matrix4x4 LocalTransform;

		/// @brief The global transform matrix
		Matrix4x4 GlobalTransform;

		/// @brief The inverse of the global transform matrix
		Matrix4x4 InvGlobalTransform;

		Transform2D();
		Transform2D(const Vector2& position, double rotation, const Vector2& scale, const Transform2D* parent = nullptr);

		/// @brief Calculates the local transform matrix
		void CalculateLocalMatrix();

		/// @brief Calculates the global transform matrix, using an optional parent's global transform.
		/// NOTE: If using a parent, make sure the parent's global transform is updated before calling this
		/// @param parent If given, this global matrix will be calculated as a child of the parent transform
		void CalculateGlobalMatrix(const Transform2D* parent = nullptr);

		/// @brief Calculates the global and local transform matrices
		/// @param parent If given, this transform will be calculated as a child of the parent transform
		void Recalculate(const Transform2D* parent = nullptr);

		/// @brief Converts a position from global to local coordinate space
		/// @param position The position, in global space
		/// @return The position in local space
		Vector2 GlobalToLocalPosition(const Vector2& position) const;

		/// @brief Converts a rotation from global to local coordinate space
		/// @param rotation The rotation, in global space
		/// @return The rotation in local space
		double GlobalToLocalRotation(double rotation) const;

		/// @brief Converts a scale from global to local coordinate space
		/// @param scale The scale, in global space
		/// @return The scale in local space
		Vector2 GlobalToLocalScale(const Vector2& scale) const;

		/// @brief Converts a directional vector from global to local coordinate space
		/// @param vector The vector, in global space
		/// @return The vector in local space
		Vector2 GlobalToLocalVector(const Vector2& vector) const;

		/// @brief Converts a position from local to global coordinate space
		/// @param position The position, in local space
		/// @return The position in global space
		Vector2 LocalToGlobalPosition(const Vector2& position) const;

		/// @brief Converts a rotation from local to global coordinate space
		/// @param rotation The rotation, in local space
		/// @return The rotation in global space
		double LocalToGlobalRotation(double rotation) const;

		/// @brief Converts a scale from local to global coordinate space
		/// @param scale The scale, in local space
		/// @return The scale in global space
		Vector2 LocalToGlobalScale(const Vector2& scale) const;

		/// @brief Converts a directional vector from local to global coordinate space
		/// @param vector The vector, in local space
		/// @return The vector in global space
		Vector2 LocalToGlobalVector(const Vector2& vector) const;

		/// @brief Translates this transform in local space
		/// @param translation The translation, in local space
		void TranslateLocal(const Vector2& translation);

		/// @brief Translates this transform in global space
		/// @param translation The translation, in global space
		/// @param parent The parent, or nullptr if no parent
		void TranslateGlobal(const Vector2& translation, const Transform2D* parent = nullptr);

		/// @brief Rotates this transform in local space
		/// @param rotation The rotation, in local space
		void RotateLocal(double rotation);

		/// @brief Rotates this transform in global space
		/// @param rotation The rotation, in global space
		/// @param parent The parent, or nullptr if no parent
		void RotateGlobal(double rotation, const Transform2D* parent = nullptr);

		/// @brief Gets the global position of this transform
		/// @return The global position
		Vector3 GetGlobalPosition() const { return LocalToGlobalPosition(Vector2::Zero); }

		/// @brief Gets the global rotation of this transform
		/// @return The global rotation
		double GetGlobalRotation() const { return LocalToGlobalRotation(0.0); }

		/// @brief Gets the global scale of this transform
		/// @return The global scale
		Vector3 GetGlobalScale() const { return LocalToGlobalScale(Vector2::One); }
	};

	/// @brief Represents a 3D transformation
	struct Transform3D
	{
		/// @brief The local position
		Vector3 LocalPosition;

		/// @brief The local rotation
		Quaternion LocalRotation;

		/// @brief The local scale
		Vector3 LocalScale;

		/// @brief The local transform matrix
		Matrix4x4 LocalTransform;

		/// @brief The global transform matrix
		Matrix4x4 GlobalTransform;

		/// @brief The inverse of the global transform matrix
		Matrix4x4 InvGlobalTransform;

		Transform3D();
		Transform3D(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const Transform3D* parent = nullptr);

		/// @brief Calculates the local transform matrix
		void CalculateLocalMatrix();

		/// @brief Calculates the global transform matrix, using an optional parent's global transform.
		/// NOTE: If using a parent, make sure the parent's global transform is updated before calling this
		/// @param parent If given, this global matrix will be calculated as a child of the parent transform
		void CalculateGlobalMatrix(const Transform3D* parent = nullptr);

		/// @brief Calculates the global and local transform matrices
		/// @param parent If given, this transform will be calculated as a child of the parent transform
		void Recalculate(const Transform3D* parent = nullptr);

		/// @brief Converts a position from global to local coordinate space
		/// @param position The position, in global space
		/// @return The position in local space
		Vector3 GlobalToLocalPosition(const Vector3& position) const;

		/// @brief Converts a rotation from global to local coordinate space
		/// @param rotation The rotation, in global space
		/// @return The rotation in local space
		Quaternion GlobalToLocalRotation(const Quaternion& rotation) const;

		/// @brief Converts a directional vector from global to local coordinate space
		/// @param vector The vector, in global space
		/// @return The vector in local space
		Vector3 GlobalToLocalVector(const Vector3& vector) const;

		/// @brief Converts a scale from global to local space coordinate space
		/// @param scale The scale, in global space
		/// @return The scale in local space
		Vector3 GlobalToLocalScale(const Vector3& scale) const;

		/// @brief Converts a position from local to global coordinate space
		/// @param position The position, in local space
		/// @return The position in global space
		Vector3 LocalToGlobalPosition(const Vector3& position) const;

		/// @brief Converts a rotation from local to global coordinate space
		/// @param rotation The rotation, in local space
		/// @return The rotation in global space
		Quaternion LocalToGlobalRotation(const Quaternion& rotation) const;

		/// @brief Converts a directional vector from local to global coordinate space
		/// @param vector The vector, in local space
		/// @return The vector in global space
		Vector3 LocalToGlobalVector(const Vector3& vector) const;

		/// @brief Converts a scale from local to global coordinate space
		/// @param scale The scale, in local space
		/// @return The scale in global space
		Vector3 LocalToGlobalScale(const Vector3& scale) const;

		/// @brief Translates this transform in local space
		/// @param translation The translation, in local space
		void TranslateLocal(const Vector3& translation);

		/// @brief Translates this transform in global space
		/// @param translation The translation, in global space
		/// @param parent The parent, or nullptr if no parent
		void TranslateGlobal(const Vector3& translation, const Transform3D* parent = nullptr);

		/// @brief Rotates this transform in local space
		/// @param rotation The rotation, in local space
		void RotateLocal(const Quaternion& rotation);

		/// @brief Rotates this transform in local space
		/// @param axis The axis to rotate around, in local space
		/// @param angleRadians The amount to rotate, in radians
		void RotateLocal(const Vector3& axis, double angleRadians);

		/// @brief Rotates this transform in global space
		/// @param rotation The rotation, in global space
		/// @param parent The parent, or nullptr if no parent
		void RotateGlobal(const Quaternion& rotation, const Transform3D* parent = nullptr);

		/// @brief Rotates this transform in global space
		/// @param axis The axis to rotate around, in global space
		/// @param angleRadians The amount to rotate, in radians
		/// @param parent The parent, or nullptr if no parent
		void RotateGlobal(const Vector3& axis, double angleRadians, const Transform3D* parent = nullptr);

		/// @brief Gets the global position of this transform
		/// @return The global position
		Vector3 GetGlobalPosition() const { return LocalToGlobalPosition(Vector3::Zero); }

		/// @brief Gets the global rotation of this transform
		/// @return The global rotation
		Quaternion GetGlobalRotation() const { return LocalToGlobalRotation(Quaternion::Identity); }

		/// @brief Gets the global scale of this transform
		/// @return The global scale
		Vector3 GetGlobalScale() const { return LocalToGlobalScale(Vector3::One); }

		/// @brief Gets the global forward direction of this transform
		/// @return The global forward direction
		Vector3 GetGlobalForward() const { return GlobalTransform.GetForwardVector(); }

		/// @brief Gets the global backward direction of this transform
		/// @return The global backward direction
		Vector3 GetGlobalBackward() const { return GlobalTransform.GetBackwardVector(); }

		/// @brief Gets the global right direction of this transform
		/// @return The global right direction
		Vector3 GetGlobalRight() const { return GlobalTransform.GetRightVector(); }

		/// @brief Gets the global left direction of this transform
		/// @return The global left direction
		Vector3 GetGlobalLeft() const { return GlobalTransform.GetLeftVector(); }

		/// @brief Gets the global up direction of this transform
		/// @return The global up direction
		Vector3 GetGlobalUp() const { return GlobalTransform.GetUpVector(); }

		/// @brief Gets the global down direction of this transform
		/// @return The global down direction
		Vector3 GetGlobalDown() const { return GlobalTransform.GetDownVector(); }

		/// @brief Gets the local forward direction of this transform
		/// @return The local forward direction
		Vector3 GetLocalForward() const { return LocalTransform.GetForwardVector(); }

		/// @brief Gets the local backward direction of this transform
		/// @return The local backward direction
		Vector3 GetLocalBackward() const { return LocalTransform.GetBackwardVector(); }

		/// @brief Gets the local right direction of this transform
		/// @return The local right direction
		Vector3 GetLocalRight() const { return LocalTransform.GetRightVector(); }

		/// @brief Gets the local left direction of this transform
		/// @return The local left direction
		Vector3 GetLocalLeft() const { return LocalTransform.GetLeftVector(); }

		/// @brief Gets the local up direction of this transform
		/// @return The local up direction
		Vector3 GetLocalUp() const { return LocalTransform.GetUpVector(); }

		/// @brief Gets the local down direction of this transform
		/// @return The local down direction
		Vector3 GetLocalDown() const { return LocalTransform.GetDownVector(); }
	};
}