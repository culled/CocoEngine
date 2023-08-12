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

	/// @brief Represents a 2D vector using integer coordinates
	struct COCOAPI Vector2Int
	{
		/// @brief A zero vector (0, 0)
		static const Vector2Int Zero;

		/// @brief A vector with 1 for each axis (1, 1)
		static const Vector2Int One;

		/// @brief The X component
		int X = 0;
		
		/// @brief The Y component
		int Y = 0;

		Vector2Int() noexcept = default;
		Vector2Int(int x, int y) noexcept;
		virtual ~Vector2Int() = default;

		/// @brief Parses a Vector2Int from a string
		/// @param str The string
		/// @return The parsed Vector2Int
		static Vector2Int Parse(const string& str);

		/// @brief Converts this vector to a string
		/// @return This vector as a string
		string ToString() const { return FormattedString("{}, {}", X, Y); }

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

	/// @brief Represents a 2D vector using decimal coordinates
	struct COCOAPI Vector2
	{
		/// @brief A zero vector (0, 0)
		static const Vector2 Zero;

		/// @brief A vector with 1 for each axis (1, 1)
		static const Vector2 One;

		/// @brief A vector pointing to the right (1, 0)
		static const Vector2 Right;

		/// @brief A vector pointing to the left (-1, 0)
		static const Vector2 Left;

		/// @brief A vector pointing up (0, 1)
		static const Vector2 Up;

		/// @brief A vector pointing down (0, -1)
		static const Vector2 Down;

		/// @brief The X component
		double X = 0.0;

		/// @brief The Y component
		double Y = 0.0;

		Vector2() = default;
		Vector2(double x, double y) noexcept;
		Vector2(const Vector3& vec) noexcept;
		virtual ~Vector2() = default;

		/// @brief Parses a Vector2 from a string
		/// @param str The string
		/// @return The parsed Vector2
		static Vector2 Parse(const string& str);

		/// @brief Returns the distance between two vectors
		/// @param a The first vector
		/// @param b The second vector
		/// @return The distance between the vectors
		static double DistanceBetween(const Vector2& a, const Vector2& b) noexcept;

		/// @brief Calculates the dot product of A and B
		/// @param a The first vector
		/// @param b The second vector
		/// @return The dot product
		static double Dot(const Vector2& a, const Vector2& b) noexcept { return a.Dot(b); }

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr double GetLengthSquared() const noexcept { return X * X + Y * Y; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const noexcept { return Math::Sqrt(X * X + Y * Y); }

		/// @brief Normalizes this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		void Normalize(bool safe = true) noexcept;

		/// @brief Gets a normalized copy of this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		/// @return A normalized copy of this vector
		Vector2 Normalized(bool safe = true) const noexcept;

		/// @brief Compares if this vector equals another vector
		/// @param other The other vector
		/// @param tolerance The difference tolerance
		/// @return True if the two vectors are within the tolerance of each other
		bool Equals(const Vector2& other, double tolerance = Math::Epsilon) const noexcept 
		{ return Math::Approximately(X, other.X, tolerance) && Math::Approximately(Y, other.Y, tolerance); }

		/// @brief Calculates the dot product of this vector with another vector
		/// @param other The other vector
		/// @return The dot product of this vector and the other vector
		double Dot(const Vector2& other) const noexcept { return X * other.X + Y * other.Y; }

		Vector2 Project(const Vector2& normal) const noexcept;
		Vector2 Reflect(const Vector2& normal) const noexcept;
		Vector2 Refract(const Vector2& normal, double ior) const noexcept;

		/// @brief Converts this vector to a string
		/// @return This vector as a string
		string ToString() const { return FormattedString("{}, {}", X, Y); }

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

		bool operator==(const Vector2& other) noexcept { return Equals(other); }
		bool operator!=(const Vector2& other) noexcept { return !Equals(other); }
	};

	/// @brief Represents a 3D vector using decimal coordinates
	struct COCOAPI Vector3
	{
		/// @brief A zero vector (0, 0, 0)
		static const Vector3 Zero;

		/// @brief A vector with 1 for each axis (1, 1, 1)
		static const Vector3 One;

		/// @brief A vector pointing to the right (1, 0, 0)
		static const Vector3 Right;

		/// @brief A vector pointing to the left (-1, 0, 0)
		static const Vector3 Left;

		/// @brief A vector pointing up (0, 0, 1)
		static const Vector3 Up;

		/// @brief A vector pointing down (0, 0, -1)
		static const Vector3 Down;

		/// @brief A vector pointing forward (0, 1, 0)
		static const Vector3 Forwards;

		/// @brief A vector pointing backward (0, -1, 0)
		static const Vector3 Backwards;

		/// @brief The X component
		double X = 0.0;

		/// @brief The Y component
		double Y = 0.0;

		/// @brief The Z component
		double Z = 0.0;

		Vector3() = default;
		Vector3(double x, double y, double z) noexcept;
		Vector3(const Vector2& vec2, double z = 0.0) noexcept;
		Vector3(const struct Vector4& vec4) noexcept;
		virtual ~Vector3() = default;

		/// @brief Parses a Vector3 from a string
		/// @param str The string
		/// @return The parsed Vector3
		static Vector3 Parse(const string& str);

		/// @brief Returns the distance between two vectors
		/// @param a The first vector
		/// @param b The second vector
		/// @return The distance between the vectors
		static double DistanceBetween(const Vector3& a, const Vector3& b) noexcept;

		/// @brief Calculates the dot product of A and B
		/// @param a The first vector
		/// @param b The second vector
		/// @return The dot product
		static double Dot(const Vector3& a, const Vector3& b) noexcept { return a.Dot(b); }

		/// @brief Calculates the cross product of A and B. The resulting vector is orthogonal to both vectors
		/// @param a The first vector
		/// @param b The second vector
		/// @return The cross product
		static Vector3 Cross(const Vector3& a, const Vector3& b) noexcept { return a.Cross(b); }

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr double GetLengthSquared() const noexcept { return X * X + Y * Y + Z * Z; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const noexcept { return Math::Sqrt(X * X + Y * Y + Z * Z); }

		/// @brief Normalizes this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		void Normalize(bool safe = true) noexcept;

		/// @brief Gets a normalized copy of this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		/// @return A normalized copy of this vector
		Vector3 Normalized(bool safe = true) const noexcept;

		/// @brief Compares if this vector equals another vector
		/// @param other The other vector
		/// @param tolerance The difference tolerance
		/// @return True if the two vectors are within the tolerance of each other
		bool Equals(const Vector3& other, double tolerance = Math::Epsilon) const noexcept
		{
			return Math::Approximately(X, other.X, tolerance) && Math::Approximately(Y, other.Y, tolerance) && Math::Approximately(Z, other.Z, tolerance);
		}

		/// @brief Calculates the dot product of this vector with another vector
		/// @param other The other vector
		/// @return The dot product of this vector and the other vector
		double Dot(const Vector3& other) const noexcept { return X * other.X + Y * other.Y + Z * other.Z; }

		/// @brief Calculates the cross product of this vector with another vector. The resulting vector is orthogonal to both vectors
		/// @param other The other vector
		/// @return The cross product
		Vector3 Cross(const Vector3& other) const noexcept { return Vector3( Y * other.Z - Z * other.Y, Z * other.X - X * other.Z, X * other.Y - Y * other.X); }

		Vector3 Project(const Vector3& normal) const noexcept;
		Vector3 Reflect(const Vector3& normal) const noexcept;
		Vector3 Refract(const Vector3& normal, double ior) const noexcept;

		/// @brief Converts this vector to a string
		/// @return This vector as a string
		string ToString() const { return FormattedString("{}, {}, {}", X, Y, Z); }

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

	/// @brief Represents a 4D vector using decimal coordinates
	struct COCOAPI Vector4
	{
		/// @brief A zero vector (0, 0, 0, 0)
		static const Vector4 Zero;

		/// @brief A vector with 1 for each axis (1, 1, 1, 1)
		static const Vector4 One;

		/// @brief The X component
		double X = 0.0;

		/// @brief The Y component
		double Y = 0.0;

		/// @brief The Z component
		double Z = 0.0;

		/// @brief The W component
		double W = 0.0;

		Vector4() = default;
		Vector4(double x, double y, double z, double w) noexcept;
		Vector4(const Vector2& vec2, double z = 0.0, double w = 0.0) noexcept;
		Vector4(const Vector3& vec3, double w = 0.0) noexcept;
		virtual ~Vector4() = default;
		
		/// @brief Parses a Vector4 from a string
		/// @param str The string
		/// @return The parsed Vector4
		static Vector4 Parse(const string& str);

		/// @brief Calculates the dot product of A and B
		/// @param a The first vector
		/// @param b The second vector
		/// @return The dot product
		static double Dot(const Vector4& a, const Vector4& b) noexcept { return a.Dot(b); }

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr double GetLengthSquared() const noexcept { return X * X + Y * Y + Z * Z + W * W; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const noexcept { return Math::Sqrt(X * X + Y * Y + Z * Z + W * W); }

		/// @brief Normalizes this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		void Normalize(bool safe = true) noexcept;

		/// @brief Gets a normalized copy of this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		/// @return A normalized copy of this vector
		Vector4 Normalized(bool safe = true) const noexcept;

		/// @brief Compares if this vector equals another vector
		/// @param other The other vector
		/// @param tolerance The difference tolerance
		/// @return True if the two vectors are within the tolerance of each other
		bool Equals(const Vector4& other, double tolerance = Math::Epsilon) const noexcept
		{
			return Math::Approximately(X, other.X, tolerance) && 
				Math::Approximately(Y, other.Y, tolerance) && 
				Math::Approximately(Z, other.Z, tolerance) && 
				Math::Approximately(W, other.W, tolerance);
		}

		/// @brief Calculates the dot product of this vector with another vector
		/// @param other The other vector
		/// @return The dot product of this vector and the other vector
		double Dot(const Vector4& other) const noexcept { return X * other.X + Y * other.Y + Z * other.Z + W * other.W; }

		/// @brief Converts this vector to a string
		/// @return This vector as a string
		string ToString() const { return FormattedString("{}, {}, {}, {}", X, Y, Z, W); }

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

		bool operator==(const Vector4& other) noexcept { return Equals(other); }
		bool operator!=(const Vector4& other) noexcept { return !Equals(other); }

		operator Color() const noexcept;
	};
}