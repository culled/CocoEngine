#pragma once

#include <Coco/Core/Core.h>
#include "Vector.h"

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

		/// <summary>
		/// Calculates the dot product of two quaternions
		/// </summary>
		/// <param name="a">The first quaternion</param>
		/// <param name="b">The second quaternion</param>
		/// <returns>The dot product</returns>
		static double Dot(const Quaternion& a, const Quaternion& b);

		/// <summary>
		/// Gets the normal of this quaternion
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the quaternion has a non-zero normal</param>
		/// <returns>The normal</returns>
		double Normal(bool safe = true) const;

		/// <summary>
		/// Normalizes this quaternion
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the quaternion has a non-zero normal</param>
		void Normalize(bool safe = true);

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

		Quaternion operator*(const Quaternion& other) const;
	};
}