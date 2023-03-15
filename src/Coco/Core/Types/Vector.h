#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	struct Vector2;

	/// <summary>
	/// Represents a 2D vector using integers
	/// </summary>
	struct COCOAPI Vector2Int
	{
		static const Vector2Int Zero;
		static const Vector2Int One;

		int X = 0;
		int Y = 0;

		Vector2Int() = default;
		Vector2Int(int x, int y) noexcept;

		Vector2Int operator+(const Vector2Int& other) const noexcept;
		Vector2Int operator-(const Vector2Int& other) const noexcept;
		Vector2Int operator*(int scalar) const noexcept;
		bool operator==(const Vector2Int& other) const noexcept;
		bool operator!= (const Vector2Int& other) const noexcept;

		operator Vector2() const noexcept;
	};

	/// <summary>
	/// Represents a 2D vector
	/// </summary>
	struct COCOAPI Vector2
	{
		static const Vector2 Zero;
		static const Vector2 One;

		static const Vector2 Right;
		static const Vector2 Left;
		static const Vector2 Up;
		static const Vector2 Down;

		double X = 0.0;
		double Y = 0.0;

		Vector2() = default;
		Vector2(double x, double y) noexcept;

		/// <summary>
		/// Returns the distance between two vectors
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The distance between the vectors</returns>
		static double DistanceBetween(const Vector2& a, const Vector2& b) noexcept;

		/// <summary>
		/// Calculates the dot product of A and B
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The dot product</returns>
		static double Dot(const Vector2& a, const Vector2& b) noexcept;

		/// <summary>
		/// Gets the squared length of this vector
		/// </summary>
		/// <returns>The squared length</returns>
		double GetLengthSquared() const noexcept;

		/// <summary>
		/// Gets the length of this vector
		/// </summary>
		/// <returns>The length</returns>
		double GetLength() const noexcept;

		/// <summary>
		/// Normalizes this vector
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the vector has a non-zero length</param>
		void Normalize(bool safe = true) noexcept;

		/// <summary>
		/// Gets a normalized copy of this vector
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the vector has a non-zero length</param>
		/// <returns>A normalized copy of this vector</returns>
		Vector2 Normalized(bool safe = true) const noexcept;

		/// <summary>
		/// Compares if this vector equals another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <param name="tolerance">The difference tolerance</param>
		/// <returns>True if the two vectors are within the tolerance of each other</returns>
		bool Equals(const Vector2& other, double tolerance) const noexcept;

		/// <summary>
		/// Calculates the dot product of this vector with another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The dot product of this vector and the other vector</returns>
		double Dot(const Vector2& other) const noexcept;

		Vector2 operator+(const Vector2& other) const noexcept;
		Vector2 operator-(const Vector2& other) const noexcept;
		Vector2 operator*(double scalar) const noexcept;
		Vector2 operator*(const Vector2& other) const noexcept;
		Vector2 operator/(double divisor) const noexcept;
		Vector2 operator/(const Vector2& other) const noexcept;
	};

	/// <summary>
	/// Represents a 3D vector
	/// </summary>
	struct COCOAPI Vector3
	{
		static const Vector3 Zero;
		static const Vector3 One;

		/// <summary>
		/// A vector pointing to the right (+X axis)
		/// </summary>
		static const Vector3 Right;

		/// <summary>
		/// A vector pointing to the left (-X axis)
		/// </summary>
		static const Vector3 Left;

		/// <summary>
		/// A vector pointing up (+Z axis)
		/// </summary>
		static const Vector3 Up;

		/// <summary>
		/// A vector pointing down (-Z axis)
		/// </summary>
		static const Vector3 Down;

		/// <summary>
		/// A vector pointing forward (+Y axis)
		/// </summary>
		static const Vector3 Forwards;

		/// <summary>
		/// A vector pointing backward (-Y axis)
		/// </summary>
		static const Vector3 Backwards;

		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;

		Vector3() = default;
		Vector3(double x, double y, double z) noexcept;
		Vector3(const Vector2& vec2, double z = 0.0) noexcept;
		Vector3(const struct Vector4& vec4) noexcept;

		/// <summary>
		/// Returns the distance between two vectors
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The distance between the vectors</returns>
		static double DistanceBetween(const Vector3& a, const Vector3& b) noexcept;

		/// <summary>
		/// Calculates the dot product of A and B
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The dot product</returns>
		static double Dot(const Vector3& a, const Vector3& b) noexcept;

		/// <summary>
		/// Calculates the cross product of A and B.
		/// The resulting vector is orthogonal to both vectors.
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The cross product</returns>
		static Vector3 Cross(const Vector3& a, const Vector3& b) noexcept;

		/// <summary>
		/// Gets the squared length of this vector
		/// </summary>
		/// <returns>The squared length</returns>
		double GetLengthSquared() const noexcept;

		/// <summary>
		/// Gets the length of this vector
		/// </summary>
		/// <returns>The length</returns>
		double GetLength() const noexcept;

		/// <summary>
		/// Normalizes this vector
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the vector has a non-zero length</param>
		void Normalize(bool safe = true) noexcept;

		/// <summary>
		/// Gets a normalized copy of this vector
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the vector has a non-zero length</param>
		/// <returns>A normalized copy of this vector</returns>
		Vector3 Normalized(bool safe = true) const noexcept;

		/// <summary>
		/// Compares if this vector equals another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <param name="tolerance">The difference tolerance</param>
		/// <returns>True if the two vectors are within the tolerance of each other</returns>
		bool Equals(const Vector3& other, double tolerance) const noexcept;

		/// <summary>
		/// Calculates the dot product of this vector with another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The dot product of this vector and the other vector</returns>
		double Dot(const Vector3& other) const noexcept;

		/// <summary>
		/// Calculates the cross product of this vector with another vector.
		/// The resulting vector is orthogonal to both vectors.
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The cross product</returns>
		Vector3 Cross(const Vector3& other) const noexcept;

		Vector3 operator+(const Vector3& other) const noexcept;
		void operator+=(const Vector3& other) noexcept;

		Vector3 operator-(const Vector3& other) const noexcept;
		void operator-=(const Vector3& other) noexcept;

		Vector3 operator*(double scalar) const noexcept;
		void operator*=(double scalar) noexcept;

		Vector3 operator*(const Vector3& other) const noexcept;
		void operator*=(const Vector3& other) noexcept;

		Vector3 operator/(double divisor) const noexcept;
		void operator/=(double divisor) noexcept;

		Vector3 operator/(const Vector3& other) const noexcept;
		void operator/=(const Vector3& other) noexcept;

		Vector3 operator-() const noexcept;
	};

	/// <summary>
	/// Represents a 4D vector
	/// </summary>
	struct COCOAPI Vector4
	{
		static const Vector4 Zero;
		static const Vector4 One;

		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;
		double W = 0.0;

		Vector4() = default;
		Vector4(double x, double y, double z, double w) noexcept;
		Vector4(const Vector2& vec2, double z = 0.0, double w = 0.0) noexcept;
		Vector4(const Vector3& vec3, double w = 0.0) noexcept;

		/// <summary>
		/// Calculates the dot product of A and B
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The dot product</returns>
		static double Dot(const Vector4& a, const Vector4& b) noexcept;

		/// <summary>
		/// Gets the squared length of this vector
		/// </summary>
		/// <returns>The squared length</returns>
		double GetLengthSquared() const noexcept;

		/// <summary>
		/// Gets the length of this vector
		/// </summary>
		/// <returns>The length</returns>
		double GetLength() const noexcept;

		/// <summary>
		/// Normalizes this vector
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the vector has a non-zero length</param>
		void Normalize(bool safe = true) noexcept;

		/// <summary>
		/// Gets a normalized copy of this vector
		/// </summary>
		/// <param name="safe">If true, a check will be done to ensure the vector has a non-zero length</param>
		/// <returns>A normalized copy of this vector</returns>
		Vector4 Normalized(bool safe = true) const noexcept;

		/// <summary>
		/// Calculates the dot product of this vector with another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The dot product of this vector and the other vector</returns>
		double Dot(const Vector4& other) const noexcept;

		Vector4 operator+(const Vector4& other) const noexcept;
		Vector4 operator-(const Vector4& other) const noexcept;
		Vector4 operator*(double scalar) const noexcept;
		Vector4 operator*(const Vector4& other) const noexcept;
		Vector4 operator/(double divisor) const noexcept;
		Vector4 operator/(const Vector4& other) const noexcept;
	};
}