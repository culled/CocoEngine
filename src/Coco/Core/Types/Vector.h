#pragma once

#include "../Defines.h"
#include "String.h"

namespace Coco
{
	/// @brief Template for Vector2s
	/// @tparam ValueType The type of values to use
	template<typename ValueType>
	struct Vector2Base
	{
	public:
		ValueType X;
		ValueType Y;

		Vector2Base() : 
			X(0),
			Y(0)
		{}

		Vector2Base(ValueType x, ValueType y) :
			X(x),
			Y(y)
		{}

		virtual ~Vector2Base() = default;

		/// @brief Gets a string representation of this vector
		/// @return The string representation
		string ToString() const
		{
			return FormatString("{}, {}", X, Y);
		}

		void operator+=(const Vector2Base& other) { X += other.X; Y += other.Y; }
		void operator-=(const Vector2Base& other) { X -= other.X; Y -= other.Y; }

		void operator*=(ValueType scalar) { X *= scalar; Y *= scalar; }
		void operator/=(ValueType scalar) { X /= scalar; Y /= scalar; }

		void operator*=(const Vector2Base& other) { X *= other.X; Y *= other.Y; }
		void operator/=(const Vector2Base& other) { X /= other.X; Y /= other.Y; }
	};

	/// @brief A Vector2 backed by ints
	struct Vector2Int : public Vector2Base<int>
	{
		/// @brief A zero-vector
		static const Vector2Int Zero;

		/// @brief A vector with one in the x and y directions
		static const Vector2Int One;

		Vector2Int() = default;
		Vector2Int(int x, int y);

		/// @brief Determines if this vector equals another
		/// @param other The other vector
		/// @return True if the two vectors are equal
		constexpr bool Equals(const Vector2Int& other) const { return X == other.X && Y == other.Y; }

		Vector2Int operator+(const Vector2Int& other) const { return Vector2Int(X + other.X, Y + other.Y); }
		Vector2Int operator-(const Vector2Int& other) const { return Vector2Int(X - other.X, Y - other.Y); }

		Vector2Int operator*(int scalar) const { return Vector2Int(X * scalar, Y * scalar); }
		Vector2Int operator/(int divisor) const { return Vector2Int(X / divisor, Y / divisor); }

		Vector2Int operator*(const Vector2Int& other) const { return Vector2Int(X * other.X, Y * other.Y); }
		Vector2Int operator/(const Vector2Int& other) const { return Vector2Int(X / other.X, Y / other.Y); }

		bool operator==(const Vector2Int& other) const { return Equals(other); }
		bool operator!=(const Vector2Int& other) const { return !Equals(other); }
	};
}