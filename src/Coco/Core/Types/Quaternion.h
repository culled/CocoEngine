#pragma once

#include <Coco/Core/Core.h>
#include "Vector.h"

namespace Coco
{
	struct Matrix4x4;

	/// <summary>
	/// Represents a 3D rotation
	/// </summary>
	struct COCOAPI Quaternion
	{
		static const Quaternion Identity;

		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;
		double W = 1.0;

		Quaternion() = default;
		Quaternion(double x, double y, double z, double w) noexcept;
		Quaternion(const Vector3& axis, double angleRadians, bool normalize = true) noexcept;
		Quaternion(const Vector3& eulerAngles, bool normalize = true) noexcept;

		/// <summary>
		/// Calculates the dot product of two quaternions
		/// </summary>
		/// <param name="a">The first quaternion</param>
		/// <param name="b">The second quaternion</param>
		/// <returns>The dot product</returns>
		static double Dot(const Quaternion& a, const Quaternion& b) noexcept;

		/// <summary>
		/// Spherically lerps from one rotation to another
		/// </summary>
		/// <param name="from">The starting rotation</param>
		/// <param name="to">The ending rotation</param>
		/// <param name="alpha">The amount between from and to to rotate, in the range [0.0, 1.0]</param>
		/// <returns>A rotation</returns>
		static Quaternion Slerp(const Quaternion& from, const Quaternion& to, double alpha) noexcept;

		/// <summary>
		/// Gets the normal of this quaternion
		/// </summary>
		/// <returns>The normal</returns>
		double Normal() const noexcept;

		/// <summary>
		/// Normalizes this quaternion
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the quaternion has a non-zero normal</param>
		void Normalize(bool safe = true) noexcept;

		/// <summary>
		/// Returns a normalized version of this quaternion
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the quaternion has a non-zero normal</param>
		/// <returns>A normalized version of this quaternion
		Quaternion Normalized(bool safe = true) const noexcept;

		/// <summary>
		/// Gets the conjugate quaternion
		/// </summary>
		/// <returns>The conjugate quaternion</returns>
		Quaternion Conjugate() const noexcept;

		/// <summary>
		/// Gets the inverse of this quaternion
		/// </summary>
		/// <returns>The inverted quaternion</returns>
		Quaternion Inverted() const noexcept;

		/// <summary>
		/// Calculates the dot product of this quaternion and another quaternion
		/// </summary>
		/// <param name="other">The other quaternion</param>
		/// <returns>The dot product</returns>
		double Dot(const Quaternion& other) const noexcept;

		/// <summary>
		/// Creates a rotation matrix from this quaternion
		/// </summary>
		/// <returns>A rotation matrix</returns>
		Matrix4x4 ToRotationMatrix() const noexcept;

		Quaternion operator*(const Quaternion& other) const noexcept;
		void operator*=(const Quaternion& other) noexcept;

		Vector3 operator*(const Vector3& direction) const noexcept;
	};
}