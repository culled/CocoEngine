#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Math/Math.h>

namespace Coco
{
	struct Matrix4x4;
	struct Vector3;

	/// @brief Represents a 3D rotation
	struct COCOAPI Quaternion
	{
		static const Quaternion Identity;

		/// @brief The X component
		double X = 0.0;

		/// @brief The Y component
		double Y = 0.0;

		/// @brief The Z component
		double Z = 0.0;

		/// @brief The W component
		double W = 1.0;

		Quaternion() noexcept = default;
		Quaternion(double x, double y, double z, double w) noexcept;
		Quaternion(const Vector3& axis, double angleRadians, bool normalize = true) noexcept;
		Quaternion(const Vector3& eulerAngles, bool normalize = true) noexcept;

		/// @brief Calculates the dot product of two quaternions
		/// @param a The first quaternion
		/// @param b The second quaternion
		/// @return The dot product
		static double Dot(const Quaternion& a, const Quaternion& b) noexcept { return a.Dot(b); }

		/// @brief Spherically interpolates from one quaternion to another
		/// @param from The starting rotation
		/// @param to The ending rotation
		/// @param alpha The amount between from and to to rotate, in the range [0.0, 1.0]
		/// @return An interpolated rotation
		static Quaternion Slerp(const Quaternion& from, const Quaternion& to, double alpha) noexcept;

		/// @brief Gets the normal of this quaternion
		/// @return The normal
		double GetNormal() const noexcept { return Math::Sqrt(X * X + Y * Y + Z * Z + W * W); }

		/// @brief Normalizes this quaternion
		/// @param safe If true, a check will be done to ensure the quaternion has a non-zero normal
		void Normalize(bool safe = true) noexcept;

		/// @brief Returns a normalized version of this quaternion
		/// @param safe If true, a check will be done to ensure the quaternion has a non-zero normal
		/// @return A normalized version of this quaternion
		Quaternion Normalized(bool safe = true) const noexcept;

		/// @brief Gets the conjugate of this quaternion
		/// @return The conjugate of this quaternion
		Quaternion Conjugate() const noexcept { return Quaternion(-X, -Y, -Z, W); }

		/// @brief Gets the inverse of this quaternion
		/// @return The inverse of this quaternion
		Quaternion Inverted() const noexcept;

		/// @brief Calculates the dot product of this quaternion and another quaternion
		/// @param other The other quaternion
		/// @return The dot product
		double Dot(const Quaternion& other) const noexcept;

		/// @brief Creates a rotation matrix from this quaternion
		/// @return A rotation matrix
		Matrix4x4 ToRotationMatrix() const noexcept;

		/// @brief Gets this rotation as euler angles
		/// @return The euler angles that this rotation represents
		Vector3 ToEulerAngles() const noexcept;

		Quaternion operator*(const Quaternion& other) const noexcept;
		void operator*=(const Quaternion& other) noexcept { *this = *this * other; }

		Vector3 operator*(const Vector3& direction) const noexcept;
	};
}