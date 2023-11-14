#pragma once

#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace Coco
{
	enum class TransformSpace
	{
		Global = 0,
		Parent,
		Self
	};

	/// @brief Represents a 2D transformation
	struct Transform2D
	{
		/// @brief The local position
		Vector2 LocalPosition;

		/// @brief The local rotation
		double LocalRotation;

		/// @brief The local scale
		Vector2 LocalScale;

		/// @brief The local transform matrix. This is how this transform is moved within its parent transform (or the world if it has no parent)
		Matrix4x4 LocalTransform;

		/// @brief The global transform matrix. This transforms local space to global space
		Matrix4x4 GlobalTransform;

		/// @brief The inverse of the global transform matrix. This transforms global space to local space, which is NOT the same as parent space
		Matrix4x4 InvGlobalTransform;

		/// @brief The parent transform matrix. This transforms parent space to global space
		Matrix4x4 ParentTransform;

		/// @brief The inverse of the parent transform matrix. This transforms global space to parent space
		Matrix4x4 InvParentTransform;

		Transform2D();
		Transform2D(const Vector2& position, double rotation, const Vector2& scale, const Transform2D* parent = nullptr);

		/// @brief Calculates the global and local transform matrices
		/// @param parent If given, this transform will be calculated as a child of the parent transform
		void Recalculate(const Transform2D* parent = nullptr);

		/// @brief Transforms a position between spaces
		/// @param position The position
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed position
		Vector2 TransformPosition(const Vector2& position, TransformSpace from, TransformSpace to) const;

		/// @brief Transforms a rotation between spaces
		/// @param rotation The rotation
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed rotation 
		double TransformRotation(double rotation, TransformSpace from, TransformSpace to) const;

		/// @brief Transforms a vector between spaces
		/// @param vector The vector
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed vector 
		Vector2 TransformVector(const Vector2& vector, TransformSpace from, TransformSpace to) const;

		/// @brief Transforms a scale between spaces
		/// @param scale The scale
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed scale 
		Vector2 TransformScale(const Vector2& scale, TransformSpace from, TransformSpace to) const;

		/// @brief Moves this transform in the given space
		/// @param translation The amount to move
		/// @param space The space to move relative to
		void Translate(const Vector2& translation, TransformSpace space);

		/// @brief Rotates this transform in the given space
		/// @param rotation The amount to rotate
		/// @param space The space to rotate relative to
		void Rotate(double rotation, TransformSpace space);

		/// @brief Sets the position in the given space
		/// @param position The position
		/// @param space The space
		void SetPosition(const Vector2& position, TransformSpace space);

		/// @brief Gets the position in the given space
		/// @param space The space
		/// @return The position
		Vector2 GetPosition(TransformSpace space) const;

		/// @brief Sets the rotation in the given space
		/// @param rotation The rotation
		/// @param space The space
		void SetRotation(double rotation, TransformSpace space);

		/// @brief Gets the rotation in the given space
		/// @param space The space
		/// @return The rotation
		double GetRotation(TransformSpace space) const;

		/// @brief Sets the scale in the given space
		/// @param scale The scale
		/// @param space The space
		void SetScale(const Vector2& scale, TransformSpace space);

		/// @brief Gets the scale in the given space
		/// @param space The space
		/// @return The scale
		Vector2 GetScale(TransformSpace space) const;

		/// @brief Decomposes this transform
		/// @param space The space
		/// @param outPosition Will be set to the position in the given space
		/// @param outRotation Will be set to the rotation in the given space
		/// @param outScale Will be set to the scale in the given space
		void Decompose(TransformSpace space, Vector2& outPosition, double& outRotation, Vector2& outScale) const;

		/// @brief Transforms a position, rotation, and scale between spaces
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @param position The position
		/// @param rotation The rotation
		/// @param scale The scale
		void Transform(TransformSpace from, TransformSpace to, Vector2& position, double& rotation, Vector2& scale) const;

		/// @brief Gets a vector that points right in the given space
		/// @param space The space
		/// @return The vector
		Vector2 GetRight(TransformSpace space) const;

		/// @brief Gets a vector that points left in the given space
		/// @param space The space
		/// @return The vector
		Vector2 GetLeft(TransformSpace space) const;

		/// @brief Gets a vector that points up in the given space
		/// @param space The space
		/// @return The vector
		Vector2 GetUp(TransformSpace space) const;

		/// @brief Gets a vector that points down in the given space
		/// @param space The space
		/// @return The vector
		Vector2 GetDown(TransformSpace space) const;

		/// @brief Gets a matrix that transforms between spaces
		/// @param from The space to transform from
		/// @param to The space to transform to 
		/// @return The transformation matrix
		Matrix4x4 GetTransformMatrix(TransformSpace from, TransformSpace to) const;
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

		/// @brief The local transform matrix. This is how this transform is moved within its parent transform (or the world if it has no parent)
		Matrix4x4 LocalTransform;

		/// @brief The global transform matrix. This transforms local space to global space
		Matrix4x4 GlobalTransform;

		/// @brief The inverse of the global transform matrix. This transforms global space to local space, which is NOT the same as parent space
		Matrix4x4 InvGlobalTransform;

		/// @brief The parent transform matrix. This transforms parent space to global space
		Matrix4x4 ParentTransform;

		/// @brief The inverse of the parent transform matrix. This transforms global space to parent space
		Matrix4x4 InvParentTransform;

		Transform3D();
		Transform3D(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const Transform3D* parent = nullptr);

		/// @brief Calculates the global and local transform matrices
		/// @param parent If given, this transform will be calculated as a child of the parent transform
		void Recalculate(const Transform3D* parent = nullptr);

		/// @brief Transforms a position between spaces
		/// @param position The position
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed position
		Vector3 TransformPosition(const Vector3& position, TransformSpace from, TransformSpace to) const;

		/// @brief Transforms a rotation between spaces
		/// @param rotation The rotation
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed rotation 
		Quaternion TransformRotation(const Quaternion& rotation, TransformSpace from, TransformSpace to) const;

		/// @brief Transforms a vector between spaces
		/// @param vector The vector
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed vector 
		Vector3 TransformVector(const Vector3& vector, TransformSpace from, TransformSpace to) const;

		/// @brief Transforms a scale between spaces
		/// @param scale The scale
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @return The transformed scale 
		Vector3 TransformScale(const Vector3& scale, TransformSpace from, TransformSpace to) const;

		/// @brief Moves this transform in the given space
		/// @param translation The amount to move
		/// @param space The space to move relative to
		void Translate(const Vector3& translation, TransformSpace space);

		/// @brief Rotates this transform in the given space
		/// @param rotation The amount to rotate
		/// @param space The space to rotate relative to
		void Rotate(const Quaternion& rotation, TransformSpace space);

		/// @brief Rotates this transform in the given space
		/// @param eulerAngles The euler angles to rotate by, in radians
		/// @param space The space to rotate relative to
		void Rotate(const Vector3& eulerAngles, TransformSpace space);

		/// @brief Rotates this transform in the given space
		/// @param axis The axis to rotate about
		/// @param angleRadians The angle to rotate by
		/// @param space The space to rotate relative to
		void Rotate(const Vector3& axis, double angleRadians, TransformSpace space);

		/// @brief Sets the position in the given space
		/// @param position The position
		/// @param space The space
		void SetPosition(const Vector3& position, TransformSpace space);

		/// @brief Gets the position in the given space
		/// @param space The space
		/// @return The position
		Vector3 GetPosition(TransformSpace space) const;

		/// @brief Sets the rotation in the given space
		/// @param rotation The rotation
		/// @param space The space
		void SetRotation(const Quaternion& rotation, TransformSpace space);

		/// @brief Gets the rotation in the given space
		/// @param space The space
		/// @return The rotation
		Quaternion GetRotation(TransformSpace space) const;

		/// @brief Sets the rotation in the given space
		/// @param eulerAngles The euler angle rotation, in radians
		/// @param space The space
		void SetEulerAngles(const Vector3& eulerAngles, TransformSpace space) { SetRotation(Quaternion(eulerAngles), space); }

		/// @brief Gets the rotation in euler angles in the given space
		/// @param space The space
		/// @return The euler angle rotation, in radians
		Vector3 GetEulerAngles(TransformSpace space) const { return GetRotation(space).ToEulerAngles(); }

		/// @brief Sets the scale in the given space
		/// @param scale The scale
		/// @param space The space
		void SetScale(const Vector3& scale, TransformSpace space);

		/// @brief Gets the scale in the given space
		/// @param space The space
		/// @return The scale
		Vector3 GetScale(TransformSpace space) const;

		/// @brief Decomposes this transform
		/// @param space The space
		/// @param outPosition Will be set to the position in the given space
		/// @param outRotation Will be set to the rotation in the given space
		/// @param outScale Will be set to the scale in the given space
		void Decompose(TransformSpace space, Vector3& outPosition, Quaternion& outRotation, Vector3& outScale) const;

		/// @brief Transforms a position, rotation, and scale between spaces
		/// @param from The space to transform from
		/// @param to The space to transform to
		/// @param position The position
		/// @param rotation The rotation
		/// @param scale The scale
		void Transform(TransformSpace from, TransformSpace to, Vector3& position, Quaternion& rotation, Vector3& scale) const;

		/// @brief Gets a vector that points forward in the given space
		/// @param space The space
		/// @return The vector
		Vector3 GetForward(TransformSpace space) const;

		/// @brief Gets a vector that points backward in the given space
		/// @param space The space
		/// @return The vector
		Vector3 GetBackward(TransformSpace space) const;

		/// @brief Gets a vector that points right in the given space
		/// @param space The space
		/// @return The vector
		Vector3 GetRight(TransformSpace space) const;

		/// @brief Gets a vector that points left in the given space
		/// @param space The space
		/// @return The vector
		Vector3 GetLeft(TransformSpace space) const;

		/// @brief Gets a vector that points up in the given space
		/// @param space The space
		/// @return The vector
		Vector3 GetUp(TransformSpace space) const;

		/// @brief Gets a vector that points down in the given space
		/// @param space The space
		/// @return The vector
		Vector3 GetDown(TransformSpace space) const;

		/// @brief Gets a matrix that transforms between spaces
		/// @param from The space to transform from
		/// @param to The space to transform to 
		/// @return The transformation matrix
		Matrix4x4 GetTransformMatrix(TransformSpace from, TransformSpace to) const;
	};
}