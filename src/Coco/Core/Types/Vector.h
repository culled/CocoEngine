#pragma once

#include "../Defines.h"
#include "String.h"
#include "../Math/Math.h"

namespace Coco
{
	/// @brief Represents a 2D vector using decimal coordinates
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
		constexpr void operator/=(double divisor) { X /= divisor; Y /= divisor; }

		Vector2 operator*(const Vector2& other) const { return Vector2(X * other.X, Y * other.Y); }
		Vector2 operator/(const Vector2& other) const { return Vector2(X / other.X, Y / other.Y); }

		constexpr void operator*=(const Vector2& other) { X *= other.X; Y *= other.Y; }
		constexpr void operator/=(const Vector2& other) { X /= other.X; Y /= other.Y; }

		Vector2 operator-() const { return Vector2(-X, -Y); }

		/// @brief Returns the distance between two points
		/// @param p0 The first point
		/// @param p1 The second point
		/// @return The distance between the points
		static double DistanceBetween(const Vector2& p0, const Vector2& p1) { return(p0 - p1).GetLength(); }

		/// @brief Determines if this vector equals another
		/// @param other The other vector
		/// @param threshold The difference tolerance
		/// @return True if the two vectors are equal
		constexpr bool Equals(const Vector2& other, double threshold = Math::Epsilon) const
		{
			return Math::Approximately(X, other.X, threshold) &&
				Math::Approximately(Y, other.Y, threshold);
		}

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr double GetLengthSquared() const { return X * X + Y * Y; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const { return Math::Sqrt(GetLengthSquared()); }

		/// @brief Normalizes this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		void Normalize(bool safe = true);

		/// @brief Gets a normalized copy of this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		/// @return A normalized copy of this vector
		Vector2 Normalized(bool safe = true) const;

		/// @brief Calculates the dot product of this vector with another vector
		/// @param other The other vector
		/// @return The dot product of this vector and the other vector
		double Dot(const Vector2& other) const { return X * other.X + Y * other.Y; }

		/// @brief Projects this vector along a normal vector.
		/// @param normal The normal vector
		/// @return The projected vector
		Vector2 Project(const Vector2& normal) const;

		/// @brief Calculates this vector reflected on a plane with the given normal
		/// @param normal The normal vector
		/// @return The reflection vector
		Vector2 Reflect(const Vector2& normal) const;

		/// @brief Calculates this vector refracted through a plane with the given normal
		/// @param normal The normal vector
		/// @param ior The index of refraction
		/// @return The refraction vector
		//Vector2 Refract(const Vector2& normal, double ior) const;

		/// @brief Gets the string representation of this vector
		/// @return This vector as a string
		string ToString() const;
	};

	/// @brief Represents a 2D vector using integer coordinates
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
		constexpr void operator/=(int divisor) { X /= divisor; Y /= divisor; }

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
		static double DistanceBetween(const Vector2Int& p0, const Vector2Int& p1) { return(p0 - p1).GetLength(); }

		/// @brief Determines if this vector equals another
		/// @param other The other vector
		/// @return True if the two vectors are equal
		constexpr bool Equals(const Vector2Int& other) const { return X == other.X && Y == other.Y; }

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr int GetLengthSquared() const { return X * X + Y * Y; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const { return Math::Sqrt(GetLengthSquared()); }

		/// @brief Gets the string representation of this vector
		/// @return This vector as a string
		string ToString() const;
	};

	/// @brief Represents a 3D vector using decimal coordinates
	struct Vector3
	{
		/// @brief A zero vector (0, 0, 0)
		static const Vector3 Zero;

		/// @brief A vector with 1 for each axis (1, 1, 1)
		static const Vector3 One;

		/// @brief A vector pointing to the right (1, 0, 0)
		static const Vector3 Right;

		/// @brief A vector pointing to the left (-1, 0, 0)
		static const Vector3 Left;

		/// @brief A vector pointing up (0, 1, 0)
		static const Vector3 Up;

		/// @brief A vector pointing down (0, -1, 0)
		static const Vector3 Down;

		/// @brief A vector pointing forward (0, 0, -1)
		static const Vector3 Forward;

		/// @brief A vector pointing backward (0, 0, 1)
		static const Vector3 Backward;

		/// @brief The X component
		double X;

		/// @brief The Y component
		double Y;

		/// @brief The Z component
		double Z;

		Vector3();
		Vector3(double x, double y, double z);
		Vector3(const Vector2& vec2, double z = 0.0);

		Vector3 operator+(const Vector3& other) const { return Vector3(X + other.X, Y + other.Y, Z + other.Z); }
		Vector3 operator-(const Vector3& other) const { return Vector3(X - other.X, Y - other.Y, Z - other.Z); }

		constexpr void operator+=(const Vector3& other) { X += other.X; Y += other.Y; Z += other.Z; }
		constexpr void operator-=(const Vector3& other) { X -= other.X; Y -= other.Y; Z -= other.Z; }

		Vector3 operator*(const Vector3& other) const { return Vector3(X * other.X, Y * other.Y, Z * other.Z); }
		Vector3 operator*(double scalar) const { return Vector3(X * scalar, Y * scalar, Z * scalar); }

		constexpr void operator*=(const Vector3& other) { X *= other.X; Y *= other.Y; Z *= other.Z; }
		constexpr void operator*=(double scalar) { X *= scalar; Y *= scalar; Z *= scalar; }

		Vector3 operator/(const Vector3& other) const { return Vector3(X / other.X, Y / other.Y, Z / other.Z); }
		Vector3 operator/(double divisor) const { return Vector3(X / divisor, Y / divisor, Z / divisor); }

		constexpr void operator/=(const Vector3& other) { X /= other.X; Y /= other.Y; Z /= other.Z; }
		constexpr void operator/=(double divisor) { X /= divisor; Y /= divisor; Z /= divisor; }

		Vector3 operator-() const { return Vector3(-X, -Y, -Z); }

		/// @brief Returns the distance between two points
		/// @param p0 The first point
		/// @param p1 The second point
		/// @return The distance between the point
		static double DistanceBetween(const Vector3& p0, const Vector3& p1) { return(p0 - p1).GetLength(); }

		/// @brief Compares if this vector equals another vector
		/// @param other The other vector
		/// @param tolerance The difference tolerance
		/// @return True if the two vectors are within the tolerance of each other
		constexpr bool Equals(const Vector3& other, double tolerance = Math::Epsilon) const
		{
			return Math::Approximately(X, other.X, tolerance) &&
				Math::Approximately(Y, other.Y, tolerance) &&
				Math::Approximately(Z, other.Z, tolerance);
		}

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr double GetLengthSquared() const { return X * X + Y * Y + Z * Z; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const { return Math::Sqrt(X * X + Y * Y + Z * Z); }

		/// @brief Normalizes this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		void Normalize(bool safe = true);

		/// @brief Gets a normalized copy of this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		/// @return A normalized copy of this vector
		Vector3 Normalized(bool safe = true) const;

		/// @brief Calculates the dot product of this vector with another vector
		/// @param other The other vector
		/// @return The dot product of this vector and the other vector
		constexpr double Dot(const Vector3& other) const { return X * other.X + Y * other.Y + Z * other.Z; }

		/// @brief Calculates the cross product of this vector with another vector. The resulting vector is orthogonal to both vectors
		/// @param other The other vector
		/// @return The cross product
		Vector3 Cross(const Vector3& other) const 
		{ 
			return Vector3(
				Y * other.Z - Z * other.Y, 
				Z * other.X - X * other.Z, 
				X * other.Y - Y * other.X); 
		}

		/// @brief Projects this vector along a normal vector.
		/// @param normal The normal vector
		/// @return The projected vector
		Vector3 Project(const Vector3& normal) const;

		/// @brief Calculates this vector reflected on a plane with the given normal
		/// @param normal The normal vector
		/// @return The reflection vector
		Vector3 Reflect(const Vector3& normal) const;

		/// @brief Calculates this vector refracted through a plane with the given normal
		/// @param normal The normal vector
		/// @param ior The index of refraction
		/// @return The refraction vector
		//Vector3 Refract(const Vector3& normal, double ior) const;

		/// @brief Converts this vector to a string
		/// @return This vector as a string
		string ToString() const;
	};

	/// @brief Represents a 3D vector using integer coordinates
	struct Vector3Int
	{
		/// @brief A zero-vector (0, 0, 0)
		static const Vector3Int Zero;

		/// @brief A vector with one in each axis (1, 1, 1)
		static const Vector3Int One;

		/// @brief A vector pointing to the right (1, 0, 0)
		static const Vector3Int Right;

		/// @brief A vector pointing to the left (-1, 0, 0)
		static const Vector3Int Left;

		/// @brief A vector pointing upwards (0, 1, 0)
		static const Vector3Int Up;

		/// @brief A vector pointing downwards (0, -1, 0)
		static const Vector3Int Down;

		/// @brief A vector pointing forward (0, 0, -1)
		static const Vector3Int Forward;

		/// @brief A vector pointing backwards (0, 0, 1)
		static const Vector3Int Backward;

		/// @brief The x value
		int X;

		/// @brief The y value
		int Y;

		/// @brief The z value
		int Z;

		Vector3Int();
		Vector3Int(int x, int y, int z);

		Vector3Int operator+(const Vector3Int& other) const { return Vector3Int(X + other.X, Y + other.Y, Z + other.Z); }
		Vector3Int operator-(const Vector3Int& other) const { return Vector3Int(X - other.X, Y - other.Y, Z - other.Z); }

		constexpr void operator+=(const Vector3Int& other) { X += other.X; Y += other.Y; Z += other.Z; }
		constexpr void operator-=(const Vector3Int& other) { X -= other.X; Y -= other.Y; Z -= other.Z; }

		Vector3Int operator*(int scalar) const { return Vector3Int(X * scalar, Y * scalar, Z * scalar); }
		Vector3Int operator/(int divisor) const { return Vector3Int(X / divisor, Y / divisor, Z / divisor); }

		constexpr void operator*=(int scalar) { X *= scalar; Y *= scalar; Z *= scalar; }
		constexpr void operator/=(int divisor) { X /= divisor; Y /= divisor; Z /= divisor; }

		Vector3Int operator*(const Vector3Int& other) const { return Vector3Int(X * other.X, Y * other.Y, Z * other.Z); }
		Vector3Int operator/(const Vector3Int& other) const { return Vector3Int(X / other.X, Y / other.Y, Z / other.Z); }

		constexpr void operator*=(const Vector3Int& other) { X *= other.X; Y *= other.Y; Z *= other.Z; }
		constexpr void operator/=(const Vector3Int& other) { X /= other.X; Y /= other.Y; Z /= other.Z; }

		constexpr bool operator==(const Vector3Int& other) const { return Equals(other); }
		constexpr bool operator!=(const Vector3Int& other) const { return !Equals(other); }

		Vector3Int operator-() const { return Vector3Int(-X, -Y, -Z); }

		operator Vector3() const { return Vector3(X, Y, Z); }

		/// @brief Returns the distance between two points
		/// @param p0 The first point
		/// @param p1 The second point
		/// @return The distance between the points
		static double DistanceBetween(const Vector3Int& p0, const Vector3Int& p1) { return (p0 - p1).GetLength(); }

		/// @brief Determines if this vector equals another
		/// @param other The other vector
		/// @return True if the two vectors are equal
		constexpr bool Equals(const Vector3Int& other) const { return X == other.X && Y == other.Y && Z == other.Z; }

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr int GetLengthSquared() const { return X * X + Y * Y + Z * Z; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const { return Math::Sqrt(GetLengthSquared()); }

		/// @brief Gets the string representation of this vector
		/// @return This vector as a string
		string ToString() const;
	};

	/// @brief Represents a 4D vector using decimal coordinates
	struct Vector4
	{
		/// @brief A zero vector (0, 0, 0, 0)
		static const Vector4 Zero;

		/// @brief A vector with 1 for each axis (1, 1, 1, 1)
		static const Vector4 One;

		/// @brief The X component
		double X;

		/// @brief The Y component
		double Y;

		/// @brief The Z component
		double Z;

		/// @brief The W component
		double W;

		Vector4();
		Vector4(double x, double y, double z, double w);
		Vector4(const Vector2& vec2, double z = 0.0, double w = 0.0);
		Vector4(const Vector3& vec3, double w = 0.0);

		Vector4 operator+(const Vector4& other) const { return Vector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W); }
		Vector4 operator-(const Vector4& other) const { return Vector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W); }

		void operator+=(const Vector4& other) { X += other.X; Y += other.Y; Z += other.Z; W += other.W; }
		void operator-=(const Vector4& other) { X -= other.X; Y -= other.Y; Z -= other.Z; W -= other.W; }

		Vector4 operator*(const Vector4& other) const { return Vector4(X * other.X, Y * other.Y, Z * other.Z, W * other.W); }
		Vector4 operator*(double scalar) const { return Vector4(X * scalar, Y * scalar, Z * scalar, W * scalar); }

		void operator*=(const Vector4& other) { X *= other.X; Y *= other.Y; Z *= other.Z; W *= other.W; }
		void operator*=(double scalar) { X *= scalar; Y *= scalar; Z *= scalar; W *= scalar; }

		Vector4 operator/(const Vector4& other) const { return Vector4(X / other.X, Y / other.Y, Z / other.Z, W / other.W); }
		Vector4 operator/(double divisor) const { return Vector4(X / divisor, Y / divisor, Z / divisor, W / divisor); }

		void operator/=(const Vector4& other) { X /= other.X; Y /= other.Y; Z /= other.Z; W /= other.W; }
		void operator/=(double divisor) { X /= divisor; Y /= divisor; Z /= divisor; W /= divisor; }

		Vector4 operator-() const { return Vector4(-X, -Y, -Z, -W); }

		/// @brief Returns the distance between two points
		/// @param p0 The first point
		/// @param p1 The second point
		/// @return The distance between the point
		static double DistanceBetween(const Vector4& p0, const Vector4& p1);

		/// @brief Compares if this vector equals another vector
		/// @param other The other vector
		/// @param tolerance The difference tolerance
		/// @return True if the two vectors are within the tolerance of each other
		constexpr bool Equals(const Vector4& other, double tolerance = Math::Epsilon) const
		{
			return Math::Approximately(X, other.X, tolerance) &&
				Math::Approximately(Y, other.Y, tolerance) &&
				Math::Approximately(Z, other.Z, tolerance) &&
				Math::Approximately(W, other.W, tolerance);
		}

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr double GetLengthSquared() const { return X * X + Y * Y + Z * Z + W * W; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const { return Math::Sqrt(X * X + Y * Y + Z * Z + W * W); }

		/// @brief Normalizes this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		void Normalize(bool safe = true);

		/// @brief Gets a normalized copy of this vector
		/// @param safe If true, a check will be done to ensure the vector has a non-zero length
		/// @return A normalized copy of this vector
		Vector4 Normalized(bool safe = true) const;

		/// @brief Calculates the dot product of this vector with another vector
		/// @param other The other vector
		/// @return The dot product of this vector and the other vector
		constexpr double Dot(const Vector4& other) const { return X * other.X + Y * other.Y + Z * other.Z + W * other.W; }

		/// @brief Converts this vector to a string
		/// @return This vector as a string
		string ToString() const;
	};

	/// @brief Represents a 4D vector using integer coordinates
	struct Vector4Int
	{
		/// @brief A zero-vector (0, 0, 0, 0)
		static const Vector4Int Zero;

		/// @brief A vector with one in each axis (1, 1, 1, 1)
		static const Vector4Int One;

		/// @brief The x value
		int X;

		/// @brief The y value
		int Y;

		/// @brief The z value
		int Z;

		/// @brief The w value
		int W;

		Vector4Int();
		Vector4Int(int x, int y, int z, int w);

		Vector4Int operator+(const Vector4Int& other) const { return Vector4Int(X + other.X, Y + other.Y, Z + other.Z, W + other.W); }
		Vector4Int operator-(const Vector4Int& other) const { return Vector4Int(X - other.X, Y - other.Y, Z - other.Z, W - other.W); }

		constexpr void operator+=(const Vector4Int& other) { X += other.X; Y += other.Y; Z += other.Z; W += other.W; }
		constexpr void operator-=(const Vector4Int& other) { X -= other.X; Y -= other.Y; Z -= other.Z; W -= other.W; }

		Vector4Int operator*(int scalar) const { return Vector4Int(X * scalar, Y * scalar, Z * scalar, W * scalar); }
		Vector4Int operator/(int divisor) const { return Vector4Int(X / divisor, Y / divisor, Z / divisor, W / divisor); }

		constexpr void operator*=(int scalar) { X *= scalar; Y *= scalar; Z *= scalar; W *= scalar; }
		constexpr void operator/=(int divisor) { X /= divisor; Y /= divisor; Z /= divisor; W /= divisor; }

		Vector4Int operator*(const Vector4Int& other) const { return Vector4Int(X * other.X, Y * other.Y, Z * other.Z, W * other.W); }
		Vector4Int operator/(const Vector4Int& other) const { return Vector4Int(X / other.X, Y / other.Y, Z / other.Z, W / other.W); }

		constexpr void operator*=(const Vector4Int& other) { X *= other.X; Y *= other.Y; Z *= other.Z; W *= other.W; }
		constexpr void operator/=(const Vector4Int& other) { X /= other.X; Y /= other.Y; Z /= other.Z; W /= other.W; }

		constexpr bool operator==(const Vector4Int& other) const { return Equals(other); }
		constexpr bool operator!=(const Vector4Int& other) const { return !Equals(other); }

		Vector4Int operator-() const { return Vector4Int(-X, -Y, -Z, -W); }

		operator Vector4() const { return Vector4(X, Y, Z, W); }

		/// @brief Returns the distance between two points
		/// @param p0 The first point
		/// @param p1 The second point
		/// @return The distance between the points
		static double DistanceBetween(const Vector4Int& p0, const Vector4Int& p1) { return (p0 - p1).GetLength(); }

		/// @brief Determines if this vector equals another
		/// @param other The other vector
		/// @return True if the two vectors are equal
		constexpr bool Equals(const Vector4Int& other) const { return X == other.X && Y == other.Y && Z == other.Z && W == other.W; }

		/// @brief Gets the squared length of this vector
		/// @return The squared length
		constexpr int GetLengthSquared() const { return X * X + Y * Y + Z * Z + W * W; }

		/// @brief Gets the length of this vector
		/// @return The length
		double GetLength() const { return Math::Sqrt(GetLengthSquared()); }

		/// @brief Gets the string representation of this vector
		/// @return This vector as a string
		string ToString() const;
	};
}