#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Math/Math.h>
#include "String.h"

namespace Coco
{
	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Color;

	/// <summary>
	/// Represents a 2D vector using integer coordinates
	/// </summary>
	struct COCOAPI Vector2Int
	{
		/// <summary>
		/// A zero vector2int (0, 0)
		/// </summary>
		static const Vector2Int Zero;

		/// <summary>
		/// A vector2int with 1 for each axis (1, 1)
		/// </summary>
		static const Vector2Int One;

		int X = 0;
		int Y = 0;

		Vector2Int() noexcept = default;
		Vector2Int(int x, int y) noexcept;
		virtual ~Vector2Int() = default;

		/// <summary>
		/// Parses a vector2int from a string
		/// </summary>
		/// <param name="str">The string</param>
		/// <returns>The parsed vector2int</returns>
		static Vector2Int Parse(const string& str);

		/// <summary>
		/// Converts this vector to a string
		/// </summary>
		/// <returns>This vector as a string</returns>
		string ToString() const noexcept { return FormattedString("{}, {}", X, Y); }

		Vector2Int operator+(const Vector2Int& other) const noexcept { return Vector2Int(X + other.X, Y + other.Y); }
		void operator+=(const Vector2Int& other) noexcept { X += other.X; Y += other.Y; }

		Vector2Int operator-(const Vector2Int& other) const noexcept { return Vector2Int(X - other.X, Y - other.Y); }
		void operator-=(const Vector2Int& other) noexcept { X -= other.X; Y -= other.Y; }

		Vector2Int operator*(const Vector2Int& other) const noexcept { return Vector2Int(X * other.X, Y * other.Y); }
		void operator*=(const Vector2Int& other) noexcept { X *= other.X; Y *= other.Y; }

		Vector2Int operator*(int scalar) const noexcept { return Vector2Int(X * scalar, Y * scalar); }
		void operator*=(int scalar) noexcept { X *= scalar; Y *= scalar; }

		Vector2Int operator/(const Vector2Int& other) const noexcept { return Vector2Int(X / other.X, Y / other.Y); }
		void operator/=(const Vector2Int& other) noexcept { X /= other.X; Y /= other.Y; }

		Vector2Int operator/(int divisor) const noexcept { return Vector2Int(X / divisor, Y / divisor); }
		void operator/=(int divisor) noexcept { X /= divisor; Y /= divisor; }

		Vector2Int operator-() const noexcept { return Vector2Int(-X, -Y); }

		bool operator==(const Vector2Int& other) const noexcept { return X == other.X && Y == other.Y; }
		bool operator!= (const Vector2Int& other) const noexcept { return X != other.X || Y != other.Y; }

		operator Vector2() const noexcept;
	};

	/// <summary>
	/// Represents a 2D vector
	/// </summary>
	struct COCOAPI Vector2
	{
		/// <summary>
		/// A zero vector2 (0, 0)
		/// </summary>
		static const Vector2 Zero;

		/// <summary>
		/// A vector2 with 1 for each axis (1, 1)
		/// </summary>
		static const Vector2 One;

		/// <summary>
		/// A vector2 pointing to the right (1, 0)
		/// </summary>
		static const Vector2 Right;

		/// <summary>
		/// A vector2 pointing to the left (-1, 0)
		/// </summary>
		static const Vector2 Left;

		/// <summary>
		/// A vector2 pointing up (0, 1)
		/// </summary>
		static const Vector2 Up;

		/// <summary>
		/// A vector2 pointing down (0, -1)
		/// </summary>
		static const Vector2 Down;

		double X = 0.0;
		double Y = 0.0;

		Vector2() = default;
		Vector2(double x, double y) noexcept;
		Vector2(const Vector3& vec) noexcept;
		virtual ~Vector2() = default;

		/// <summary>
		/// Parses a vector2 from a string
		/// </summary>
		/// <param name="str">The string</param>
		/// <returns>The parsed vector2</returns>
		static Vector2 Parse(const string& str);

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
		static double Dot(const Vector2& a, const Vector2& b) noexcept { return a.Dot(b); }

		/// <summary>
		/// Gets the squared length of this vector
		/// </summary>
		/// <returns>The squared length</returns>
		double GetLengthSquared() const noexcept { return X * X + Y * Y; }

		/// <summary>
		/// Gets the length of this vector
		/// </summary>
		/// <returns>The length</returns>
		double GetLength() const noexcept { return Math::Sqrt(X * X + Y * Y); }

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
		bool Equals(const Vector2& other, double tolerance = Math::Epsilon) const noexcept 
		{ return Math::Approximately(X, other.X, tolerance) && Math::Approximately(Y, other.Y, tolerance); }

		/// <summary>
		/// Calculates the dot product of this vector with another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The dot product of this vector and the other vector</returns>
		double Dot(const Vector2& other) const noexcept { return X * other.X + Y * other.Y; }

		/// <summary>
		/// Converts this vector to a string
		/// </summary>
		/// <returns>This vector as a string</returns>
		string ToString() const noexcept { return FormattedString("{}, {}", X, Y); }

		Vector2 operator+(const Vector2& other) const noexcept { return Vector2(X + other.X, Y + other.Y); }
		void operator+=(const Vector2& other) noexcept { X += other.X; Y += other.Y; }

		Vector2 operator-(const Vector2& other) const noexcept { return Vector2(X - other.X, Y - other.Y); }
		void operator-=(const Vector2& other) noexcept { X -= other.X; Y -= other.Y; }

		Vector2 operator*(const Vector2& other) const noexcept { return Vector2(X * other.X, Y * other.Y); }
		void operator*=(const Vector2& other) noexcept { X *= other.X; Y *= other.Y; }

		Vector2 operator*(double scalar) const noexcept { return Vector2(X * scalar, Y * scalar); }
		void operator*=(double scalar) noexcept { X *= scalar; Y *= scalar; }

		Vector2 operator/(double divisor) const noexcept { return Vector2(X / divisor, Y / divisor); }
		void operator/=(double divisor) noexcept { X /= divisor; Y /= divisor; }

		Vector2 operator-() const noexcept { return Vector2(-X, -Y); }

		operator Vector3() const noexcept;

		bool operator==(const Vector2& other) { return Equals(other); }
		bool operator!=(const Vector2& other) { return !Equals(other); }
	};

	/// <summary>
	/// Represents a 3D vector
	/// </summary>
	struct COCOAPI Vector3
	{
		/// <summary>
		/// A zero vector3 (0, 0, 0)
		/// </summary>
		static const Vector3 Zero;

		/// <summary>
		/// A vector3 with 1 for each axis (1, 1, 1)
		/// </summary>
		static const Vector3 One;

		/// <summary>
		/// A vector3 pointing to the right (1, 0, 0)
		/// </summary>
		static const Vector3 Right;

		/// <summary>
		/// A vector3 pointing to the left (-1, 0, 0)
		/// </summary>
		static const Vector3 Left;

		/// <summary>
		/// A vector3 pointing up (0, 0, 1)
		/// </summary>
		static const Vector3 Up;

		/// <summary>
		/// A vector3 pointing down (0, 0, -1)
		/// </summary>
		static const Vector3 Down;

		/// <summary>
		/// A vector3 pointing forward (0, 1, 0)
		/// </summary>
		static const Vector3 Forwards;

		/// <summary>
		/// A vector3 pointing backward (0, -1, 0)
		/// </summary>
		static const Vector3 Backwards;

		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;

		Vector3() = default;
		Vector3(double x, double y, double z) noexcept;
		Vector3(const Vector2& vec2, double z = 0.0) noexcept;
		Vector3(const struct Vector4& vec4) noexcept;
		virtual ~Vector3() = default;

		/// <summary>
		/// Parses a vector3 from a string
		/// </summary>
		/// <param name="str">The string</param>
		/// <returns>The parsed vector3</returns>
		static Vector3 Parse(const string& str);

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
		static double Dot(const Vector3& a, const Vector3& b) noexcept { return a.Dot(b); }

		/// <summary>
		/// Calculates the cross product of A and B.
		/// The resulting vector is orthogonal to both vectors.
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The cross product</returns>
		static Vector3 Cross(const Vector3& a, const Vector3& b) noexcept { return a.Cross(b); }

		/// <summary>
		/// Gets the squared length of this vector
		/// </summary>
		/// <returns>The squared length</returns>
		double GetLengthSquared() const noexcept { return X * X + Y * Y + Z * Z; }

		/// <summary>
		/// Gets the length of this vector
		/// </summary>
		/// <returns>The length</returns>
		double GetLength() const noexcept { return Math::Sqrt(X * X + Y * Y + Z * Z); }

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
		bool Equals(const Vector3& other, double tolerance = Math::Epsilon) const noexcept
		{
			return Math::Approximately(X, other.X, tolerance) && Math::Approximately(Y, other.Y, tolerance) && Math::Approximately(Z, other.Z, tolerance);
		}

		/// <summary>
		/// Calculates the dot product of this vector with another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The dot product of this vector and the other vector</returns>
		double Dot(const Vector3& other) const noexcept { return X * other.X + Y * other.Y + Z * other.Z; }

		/// <summary>
		/// Calculates the cross product of this vector with another vector.
		/// The resulting vector is orthogonal to both vectors.
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The cross product</returns>
		Vector3 Cross(const Vector3& other) const noexcept { return Vector3( Y * other.Z - Z * other.Y, Z * other.X - X * other.Z, X * other.Y - Y * other.X); }

		/// <summary>
		/// Converts this vector to a string
		/// </summary>
		/// <returns>This vector as a string</returns>
		string ToString() const noexcept { return FormattedString("{}, {}, {}", X, Y, Z); }

		Vector3 operator+(const Vector3& other) const noexcept { return Vector3(X + other.X, Y + other.Y, Z + other.Z); }
		void operator+=(const Vector3& other) noexcept { X += other.X; Y += other.Y; Z += other.Z; }

		Vector3 operator-(const Vector3& other) const noexcept { return Vector3(X - other.X, Y - other.Y, Z - other.Z); }
		void operator-=(const Vector3& other) noexcept { X -= other.X; Y -= other.Y; Z -= other.Z; }

		Vector3 operator*(const Vector3& other) const noexcept { return Vector3(X * other.X, Y * other.Y, Z * other.Z); }
		void operator*=(const Vector3& other) noexcept { X *= other.X; Y *= other.Y; Z *= other.Z; }

		Vector3 operator*(double scalar) const noexcept { return Vector3(X * scalar, Y * scalar, Z * scalar); }
		void operator*=(double scalar) noexcept { X *= scalar; Y *= scalar; Z *= scalar; }

		Vector3 operator/(const Vector3& other) const noexcept { return Vector3(X / other.X, Y / other.Y, Z / other.Z); }
		void operator/=(const Vector3& other) noexcept { X /= other.X; Y /= other.Y; Z /= other.Z; }

		Vector3 operator/(double divisor) const noexcept { return Vector3(X / divisor, Y / divisor, Z / divisor); }
		void operator/=(double divisor) noexcept { X /= divisor; Y /= divisor; Z /= divisor; }

		Vector3 operator-() const noexcept { return Vector3(-X, -Y, -Z); }

		operator Vector4() const noexcept;

		bool operator==(const Vector3& other) noexcept { return Equals(other); }
		bool operator!=(const Vector3& other) noexcept { return !Equals(other); }
	};

	/// <summary>
	/// Represents a 4D vector
	/// </summary>
	struct COCOAPI Vector4
	{
		/// <summary>
		/// A zero vector4 (0, 0, 0, 0)
		/// </summary>
		static const Vector4 Zero;

		/// <summary>
		/// A vector4 with 1 for each axis (1, 1, 1, 1)
		/// </summary>
		static const Vector4 One;

		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;
		double W = 0.0;

		Vector4() = default;
		Vector4(double x, double y, double z, double w) noexcept;
		Vector4(const Vector2& vec2, double z = 0.0, double w = 0.0) noexcept;
		Vector4(const Vector3& vec3, double w = 0.0) noexcept;
		virtual ~Vector4() = default;

		/// <summary>
		/// Parses a vector4 from a string
		/// </summary>
		/// <param name="str">The string</param>
		/// <returns>The parsed vector4</returns>
		static Vector4 Parse(const string& str);

		/// <summary>
		/// Calculates the dot product of A and B
		/// </summary>
		/// <param name="a">The first vector</param>
		/// <param name="b">The second vector</param>
		/// <returns>The dot product</returns>
		static double Dot(const Vector4& a, const Vector4& b) noexcept { return a.Dot(b); }

		/// <summary>
		/// Gets the squared length of this vector
		/// </summary>
		/// <returns>The squared length</returns>
		double GetLengthSquared() const noexcept { return X * X + Y * Y + Z * Z + W * W; }

		/// <summary>
		/// Gets the length of this vector
		/// </summary>
		/// <returns>The length</returns>
		double GetLength() const noexcept { return Math::Sqrt(X * X + Y * Y + Z * Z + W * W); }

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
		/// Compares if this vector equals another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <param name="tolerance">The difference tolerance</param>
		/// <returns>True if the two vectors are within the tolerance of each other</returns>
		bool Equals(const Vector4& other, double tolerance = Math::Epsilon) const noexcept
		{
			return Math::Approximately(X, other.X, tolerance) && 
				Math::Approximately(Y, other.Y, tolerance) && 
				Math::Approximately(Z, other.Z, tolerance) && 
				Math::Approximately(W, other.W, tolerance);
		}

		/// <summary>
		/// Calculates the dot product of this vector with another vector
		/// </summary>
		/// <param name="other">The other vector</param>
		/// <returns>The dot product of this vector and the other vector</returns>
		double Dot(const Vector4& other) const noexcept { return X * other.X + Y * other.Y + Z * other.Z + W * other.W; }

		/// <summary>
		/// Converts this vector to a string
		/// </summary>
		/// <returns>This vector as a string</returns>
		string ToString() const noexcept { return FormattedString("{}, {}, {}, {}", X, Y, Z, W); }

		Vector4 operator+(const Vector4& other) const noexcept { return Vector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W); }
		void operator+=(const Vector4& other) noexcept { X += other.X; Y += other.Y; Z += other.Z; W += other.W; }

		Vector4 operator-(const Vector4& other) const noexcept { return Vector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W); }
		void operator-=(const Vector4& other) noexcept { X -= other.X; Y -= other.Y; Z -= other.Z; W -= other.W; }

		Vector4 operator*(const Vector4& other) const noexcept { return Vector4(X * other.X, Y * other.Y, Z * other.Z, W * other.W); }
		void operator*=(const Vector4& other) noexcept { X *= other.X; Y *= other.Y; Z *= other.Z; W *= other.W; }

		Vector4 operator*(double scalar) const noexcept { return Vector4(X * scalar, Y * scalar, Z * scalar, W * scalar); }
		void operator*=(double scalar) noexcept { X *= scalar; Y *= scalar; Z *= scalar; W *= scalar; }

		Vector4 operator/(const Vector4& other) const noexcept { return Vector4(X / other.X, Y / other.Y, Z / other.Z, W / other.W); }
		void operator/=(const Vector4& other) noexcept { X /= other.X; Y /= other.Y; Z /= other.Z; W /= other.W; }

		Vector4 operator/(double divisor) const noexcept { return Vector4(X / divisor, Y / divisor, Z / divisor, W / divisor); }
		void operator/=(double divisor) noexcept { X /= divisor; Y /= divisor; Z /= divisor; W /= divisor; }

		Vector4 operator-() const noexcept { return Vector4(-X, -Y, -Z, -W); }

		bool operator==(const Vector4& other) { return Equals(other); }
		bool operator!=(const Vector4& other) { return !Equals(other); }

		operator Color() const noexcept;
	};
}