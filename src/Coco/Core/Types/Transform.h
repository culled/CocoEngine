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
	};
}