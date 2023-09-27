#pragma once

#include "../Math/Math.h"

namespace Coco
{
	struct Vector3;

	/// @brief Represents a 3D rotation
	struct Quaternion
	{
		/// @brief An identity quaternion (no rotation)
		static const Quaternion Identity;

		/// @brief The X component
		double X;

		/// @brief The Y component
		double Y;

		/// @brief The Z component
		double Z;

		/// @brief The W component
		double W;

		Quaternion();
		Quaternion(double x, double y, double z, double w);
		Quaternion(const Vector3& axis, double angleRadians);
		Quaternion(const Vector3& eulerAngles);

		Quaternion operator*(const Quaternion& other) const;
		void operator*=(const Quaternion& other) { *this = *this * other; }

		Vector3 operator*(const Vector3& direction) const;

		/// @brief Spherically interpolates from one quaternion to another
		/// @param from The starting rotation
		/// @param to The ending rotation
		/// @param alpha The amount between from and to to rotate, in the range [0.0, 1.0]
		/// @return An interpolated rotation
		static Quaternion Slerp(const Quaternion& from, const Quaternion& to, double alpha);

		/// @brief Gets the normal of this quaternion
		/// @return The normal
		double GetNormal() const { return Math::Sqrt(X * X + Y * Y + Z * Z + W * W); }

		/// @brief Normalizes this quaternion
		/// @param safe If true, a check will be done to ensure the quaternion has a non-zero normal
		void Normalize(bool safe = true);

		/// @brief Returns a normalized version of this quaternion
		/// @param safe If true, a check will be done to ensure the quaternion has a non-zero normal
		/// @return A normalized version of this quaternion
		Quaternion Normalized(bool safe = true) const;

		/// @brief Gets the conjugate of this quaternion
		/// @return The conjugate of this quaternion
		Quaternion Conjugate() const { return Quaternion(-X, -Y, -Z, W); }

		/// @brief Gets the inverse of this quaternion
		/// @return The inverse of this quaternion
		Quaternion Inverted() const { return Conjugate(); }

		/// @brief Calculates the dot product of this quaternion and another quaternion
		/// @param other The other quaternion
		/// @return The dot product
		constexpr double Dot(const Quaternion& other) const { return X * other.X + Y * other.Y + Z * other.Z + W * other.W; }

		/// @brief Gets this rotation as euler angles
		/// @return The euler angles that this rotation represents
		Vector3 ToEulerAngles() const;
	};
}