#pragma once

#include <Coco/Core/Core.h>
#include "Vector.h"
#include "Matrix.h"

namespace Coco
{
	/// <summary>
	/// Represents a 3D rotation
	/// </summary>
	struct COCOAPI Quaternion
	{
		static const Quaternion Identity;

		double X, Y, Z, W;

		Quaternion();
		Quaternion(double x, double y, double z, double w);
		Quaternion(const Vector3& axis, double angleRadians, bool normalize = true);

		/// <summary>
		/// Calculates the dot product of two quaternions
		/// </summary>
		/// <param name="a">The first quaternion</param>
		/// <param name="b">The second quaternion</param>
		/// <returns>The dot product</returns>
		static double Dot(const Quaternion& a, const Quaternion& b);

		/// <summary>
		/// Creates a rotation matrix based on the rotation and center point
		/// </summary>
		/// <param name="rotation">The rotation</param>
		/// <param name="center">The center point</param>
		/// <returns>A rotation matrix</returns>
		static Matrix4x4 CreateRotationMatrix(const Quaternion& rotation, const Vector3& center);

		static Quaternion Slerp(const Quaternion& from, const Quaternion& to, double alpha);

		/// <summary>
		/// Gets the normal of this quaternion
		/// </summary>
		/// <returns>The normal</returns>
		double Normal() const;

		/// <summary>
		/// Normalizes this quaternion
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the quaternion has a non-zero normal</param>
		void Normalize(bool safe = true);

		/// <summary>
		/// Returns a normalized version of this quaternion
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the quaternion has a non-zero normal</param>
		/// <returns>A normalized version of this quaternion
		Quaternion Normalized(bool safe = true) const;

		/// <summary>
		/// Gets the conjugate quaternion
		/// </summary>
		/// <returns>The conjugate quaternion</returns>
		Quaternion Conjugate() const;

		/// <summary>
		/// Gets the inverse of this quaternion
		/// </summary>
		/// <returns>The inverted quaternion</returns>
		Quaternion Inverted() const;

		/// <summary>
		/// Calculates the dot product of this quaternion and another quaternion
		/// </summary>
		/// <param name="other">The other quaternion</param>
		/// <returns>The dot product</returns>
		double Dot(const Quaternion& other) const;

		/// <summary>
		/// Creates a rotation matrix from this quaternion
		/// </summary>
		/// <returns>A rotation matrix</returns>
		Matrix4x4 ToMatrix4x4() const;

		Quaternion operator*(const Quaternion& other) const;
		void operator*=(const Quaternion& other);
	};
}