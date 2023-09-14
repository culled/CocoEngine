#pragma once

#include "../Defines.h"
#include "String.h"
#include "../Math/Math.h"

namespace Coco
{
	/// @brief A Vector2 backed by doubles
	struct Vector2
	{
		/// @brief A zero-vector (0, 0)
		static const Vector2 Zero;

		/// @brief A vector with one in each axis (1, 1)
		static const Vector2 One;

		/// @brief A vector pointing to the right (1, 0)
		static const Vector2 Right;

		/// @brief A vector pointing to the left (-1, 0)
		static const Vector2 Left;

		/// @brief A vector pointing upwards (0, 1)
		static const Vector2 Up;

		/// @brief A vector pointing downwards (0, -1)
		static const Vector2 Down;

		/// @brief The x value
		double X;

		/// @brief The y value
		double Y;

		Vector2();
		Vector2(double x, double y);

		Vector2 operator+(const Vector2& other) const { return Vector2(X + other.X, Y + other.Y); }
		Vector2 operator-(const Vector2& other) const { return Vector2(X - other.X, Y - other.Y); }

		constexpr void operator+=(const Vector2& other) { X += other.X; Y += other.Y; }
		constexpr void operator-=(const Vector2& other) { X -= other.X; Y -= other.Y; }

		Vector2 operator*(double scalar) const { return Vector2(X * scalar, Y * scalar); }
		Vector2 operator/(double divisor) const { return Vector2(X / divisor, Y / divisor); }

		constexpr void operator*=(double scalar) { X *= scalar; Y *= scalar; }
		constexpr void operator/=(double scalar) { X /= scalar; Y /= scalar; }

		Vector2 operator*(const Vector2& other) const { return Vector2(X * other.X, Y * other.Y); }
		Vector2 operator/(const Vector2& other) const { return Vector2(X / other.X, Y / other.Y); }

		constexpr void operator*=(const Vector2& other) { X *= other.X; Y *= other.Y; }
		constexpr void operator/=(const Vector2& other) { X /= other.X; Y /= other.Y; }

		Vector2 operator-() const { return Vector2(-X, -Y); }

		/// @brief Returns the distance between two points
		/// @param p0 The first point
		/// @param p1 The second point
		/// @return The distance between the points
		static double DistanceBetween(const Vector2& p0, const Vector2& p1);

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr double GetLengthSquared() const { return X * X + Y * Y; }

		/// @brief Gets the length of this vector
		/// @return The length
		constexpr double GetLength() const { return Math::Sqrt(GetLengthSquared()); }

		/// @brief Determines if this vector equals another
		/// @param other The other vector
		/// @return True if the two vectors are equal
		constexpr bool Equals(const Vector2& other, double threshold = Math::Epsilon) const 
		{ 
			return Math::Approximately(X, other.X, threshold) && 
				Math::Approximately(Y, other.Y, threshold);
		}

		/// @brief Gets the string representation of this vector
		/// @return This vector as a string
		string ToString() const;
	};

	/// @brief A Vector2 backed by ints
	struct Vector2Int
	{
		/// @brief A zero-vector (0, 0)
		static const Vector2Int Zero;

		/// @brief A vector with one in each axis (1, 1)
		static const Vector2Int One;

		/// @brief A vector pointing to the right (1, 0)
		static const Vector2Int Right;

		/// @brief A vector pointing to the left (-1, 0)
		static const Vector2Int Left;

		/// @brief A vector pointing upwards (0, 1)
		static const Vector2Int Up;

		/// @brief A vector pointing downwards (0, -1)
		static const Vector2Int Down;

		/// @brief The x value
		int X;

		/// @brief The y value
		int Y;

		Vector2Int();
		Vector2Int(int x, int y);

		Vector2Int operator+(const Vector2Int& other) const { return Vector2Int(X + other.X, Y + other.Y); }
		Vector2Int operator-(const Vector2Int& other) const { return Vector2Int(X - other.X, Y - other.Y); }

		constexpr void operator+=(const Vector2Int& other) { X += other.X; Y += other.Y; }
		constexpr void operator-=(const Vector2Int& other) { X -= other.X; Y -= other.Y; }

		Vector2Int operator*(int scalar) const { return Vector2Int(X * scalar, Y * scalar); }
		Vector2Int operator/(int divisor) const { return Vector2Int(X / divisor, Y / divisor); }

		constexpr void operator*=(int scalar) { X *= scalar; Y *= scalar; }
		constexpr void operator/=(int scalar) { X /= scalar; Y /= scalar; }

		Vector2Int operator*(const Vector2Int& other) const { return Vector2Int(X * other.X, Y * other.Y); }
		Vector2Int operator/(const Vector2Int& other) const { return Vector2Int(X / other.X, Y / other.Y); }

		constexpr void operator*=(const Vector2Int& other) { X *= other.X; Y *= other.Y; }
		constexpr void operator/=(const Vector2Int& other) { X /= other.X; Y /= other.Y; }

		constexpr bool operator==(const Vector2Int& other) const { return Equals(other); }
		constexpr bool operator!=(const Vector2Int& other) const { return !Equals(other); }

		Vector2Int operator-() const { return Vector2Int(-X, -Y); }

		operator Vector2() const { return Vector2(X, Y); }

		/// @brief Returns the distance between two points
		/// @param p0 The first point
		/// @param p1 The second point
		/// @return The distance between the points
		static double DistanceBetween(const Vector2Int& p0, const Vector2Int& p1);

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr int GetLengthSquared() const { return X * X + Y * Y; }

		/// @brief Gets the length of this vector
		/// @return The length
		constexpr double GetLength() const { return Math::Sqrt(GetLengthSquared()); }

		/// @brief Determines if this vector equals another
		/// @param other The other vector
		/// @return True if the two vectors are equal
		constexpr bool Equals(const Vector2Int& other) const { return X == other.X && Y == other.Y; }

		/// @brief Gets the string representation of this vector
		/// @return This vector as a string
		string ToString() const;
	};
}