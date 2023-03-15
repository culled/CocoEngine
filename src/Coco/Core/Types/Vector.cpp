#include "Vector.h"

namespace Coco
{
	const Vector2Int Vector2Int::Zero = Vector2Int(0, 0);
	const Vector2Int Vector2Int::One = Vector2Int(1, 1);

	Vector2Int::Vector2Int(int x, int y) noexcept : X(x), Y(y)
	{}

	Vector2Int Vector2Int::operator+(const Vector2Int& other) const noexcept
	{
		return Vector2Int(X + other.X, Y + other.Y);
	}

	Vector2Int Vector2Int::operator-(const Vector2Int& other) const noexcept
	{
		return Vector2Int(X - other.X, Y - other.Y);
	}

	Vector2Int Vector2Int::operator*(int scalar) const noexcept
	{
		return Vector2Int(X * scalar, Y * scalar);
	}

	bool Vector2Int::operator==(const Vector2Int& other) const noexcept
	{
		return X == other.X && Y == other.Y;
	}

	bool Vector2Int::operator!= (const Vector2Int& other) const noexcept
	{
		return !(*this == other);
	}

	Vector2Int::operator Vector2() const noexcept
	{
		return Vector2(static_cast<double>(X), static_cast<double>(Y));
	}

	const Vector2 Vector2::Zero = Vector2(0.0, 0.0);
	const Vector2 Vector2::One = Vector2(1.0, 1.0);

	const Vector2 Vector2::Right = Vector2(1.0, 0.0);
	const Vector2 Vector2::Left = Vector2(-1.0, 0.0);
	const Vector2 Vector2::Up = Vector2(0.0, 1.0);
	const Vector2 Vector2::Down = Vector2(0.0, -1.0);

	Vector2::Vector2(double x, double y) noexcept :
		X(x), Y(y)
	{}

	double Vector2::DistanceBetween(const Vector2& a, const Vector2& b) noexcept
	{
		const Vector2 diff = a - b;
		return diff.GetLength();
	}

	double Vector2::Dot(const Vector2& a, const Vector2& b) noexcept
	{
		return a.Dot(b);
	}

	double Vector2::GetLengthSquared() const noexcept
	{
		return X * X + Y * Y;
	}

	double Vector2::GetLength() const noexcept
	{
		return Math::Sqrt(GetLengthSquared());
	}

	void Vector2::Normalize(bool safe) noexcept
	{
		if (safe && 
			Math::Approximately(X, 0.0) && 
			Math::Approximately(Y, 0.0))
			return;

		const double length = GetLength();
		X /= length;
		Y /= length;
	}

	Vector2 Vector2::Normalized(bool safe) const noexcept
	{
		Vector2 copy = *this;
		copy.Normalize(safe);
		return copy;
	}

	bool Vector2::Equals(const Vector2& other, double tolerance) const noexcept
	{
		if (!Math::Approximately(X, other.X, tolerance))
			return false;

		if (!Math::Approximately(Y, other.Y, tolerance))
			return false;

		return true;
	}

	double Vector2::Dot(const Vector2& other) const noexcept
	{
		return X * other.X +
			Y * other.Y;
	}

	Vector2 Vector2::operator+(const Vector2 & other) const noexcept
	{
		return Vector2(X + other.X, Y + other.Y);
	}

	Vector2 Vector2::operator-(const Vector2& other) const noexcept
	{
		return Vector2(X - other.X, Y - other.Y);
	}

	Vector2 Vector2::operator*(double scalar) const noexcept
	{
		return Vector2(X * scalar, Y * scalar);
	}

	Vector2 Vector2::operator*(const Vector2& other) const noexcept
	{
		return Vector2(X * other.X, Y * other.Y);
	}

	Vector2 Vector2::operator/(double divisor) const noexcept
	{
		return Vector2(X / divisor, Y / divisor);
	}

	Vector2 Vector2::operator/(const Vector2& other) const noexcept
	{
		return Vector2(X / other.X, Y / other.Y);
	}

	const Vector3 Vector3::Zero = Vector3(0.0, 0.0, 0.0);
	const Vector3 Vector3::One = Vector3(1.0, 1.0, 1.0);

	const Vector3 Vector3::Right = Vector3(1.0, 0.0, 0.0);
	const Vector3 Vector3::Left = Vector3(-1.0, 0.0, 0.0);
	const Vector3 Vector3::Up = Vector3(0.0, 0.0, 1.0);
	const Vector3 Vector3::Down = Vector3(0.0, 0.0, -1.0);
	const Vector3 Vector3::Forwards = Vector3(0.0, 1.0, 0.0);
	const Vector3 Vector3::Backwards = Vector3(0.0, -1.0, 0.0);

	Vector3::Vector3(double x, double y, double z) noexcept :
		X(x), Y(y), Z(z)
	{}

	Vector3::Vector3(const Vector2& vec2, double z) noexcept : Vector3(vec2.X, vec2.Y, z)
	{}

	Vector3::Vector3(const Vector4& vec4) noexcept : Vector3(vec4.X, vec4.Y, vec4.Z)
	{}

	double Vector3::DistanceBetween(const Vector3 & a, const Vector3 & b) noexcept
	{
		const Vector3 diff = a - b;
		return diff.GetLength();
	}

	double Vector3::Dot(const Vector3& a, const Vector3& b) noexcept
	{
		return a.Dot(b);
	}

	Vector3 Vector3::Cross(const Vector3& a, const Vector3& b) noexcept
	{
		return a.Cross(b);
	}

	double Vector3::GetLengthSquared() const noexcept
	{
		return X * X + Y * Y + Z * Z;
	}

	double Vector3::GetLength() const noexcept
	{
		return Math::Sqrt(GetLengthSquared());
	}

	void Vector3::Normalize(bool safe) noexcept
	{
		if (safe && 
			Math::Approximately(X, 0.0) && 
			Math::Approximately(Y, 0.0) && 
			Math::Approximately(Z, 0.0))
			return;

		const double length = GetLength();
		X /= length;
		Y /= length;
		Z /= length;
	}

	Vector3 Vector3::Normalized(bool safe) const noexcept
	{
		Vector3 copy = *this;
		copy.Normalize(safe);
		return copy;
	}

	bool Vector3::Equals(const Vector3& other, double tolerance) const noexcept
	{
		if(!Math::Approximately(X, other.X, tolerance))
			return false;

		if (!Math::Approximately(Y, other.Y, tolerance))
			return false;

		if (!Math::Approximately(Z, other.Z, tolerance))
			return false;

		return true;
	}

	double Vector3::Dot(const Vector3& other) const noexcept
	{
		return X * other.X +
			Y * other.Y +
			Z * other.Z;
	}

	Vector3 Vector3::Cross(const Vector3& other) const noexcept
	{
		return Vector3(
			Y * other.Z - Z * other.Y, 
			Z * other.X - X * other.Z, 
			X * other.Y - Y * other.X);
	}

	Vector3 Vector3::operator+(const Vector3 & other) const noexcept
	{
		return Vector3(X + other.X, Y + other.Y, Z + other.Z);
	}

	void Vector3::operator+=(const Vector3& other) noexcept
	{
		X += other.X;
		Y += other.Y;
		Z += other.Z;
	}

	Vector3 Vector3::operator-(const Vector3& other) const noexcept
	{
		return Vector3(X - other.X, Y - other.Y, Z - other.Z);
	}

	void Vector3::operator-=(const Vector3& other) noexcept
	{
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;
	}

	Vector3 Vector3::operator*(double scalar) const noexcept
	{
		return Vector3(X * scalar, Y * scalar, Z * scalar);
	}

	void Vector3::operator*=(double scalar) noexcept
	{
		X *= scalar;
		Y *= scalar;
		Z *= scalar;
	}

	Vector3 Vector3::operator*(const Vector3& other) const noexcept
	{
		return Vector3(X * other.X, Y * other.Y, Z * other.Z);
	}

	void Vector3::operator*=(const Vector3& other) noexcept
	{
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;
	}

	Vector3 Vector3::operator/(double divisor) const noexcept
	{
		return Vector3(X / divisor, Y / divisor, Z / divisor);
	}

	void Vector3::operator/=(double divisor) noexcept
	{
		X /= divisor;
		Y /= divisor;
		Z /= divisor;
	}

	Vector3 Vector3::operator/(const Vector3& other) const noexcept
	{
		return Vector3(X / other.X, Y / other.Y, Z / other.Z);
	}

	void Vector3::operator/=(const Vector3& other) noexcept
	{
		X /= other.X;
		Y /= other.Y;
		Z /= other.Z;
	}

	Vector3 Vector3::operator-() const noexcept
	{
		return Vector3(-X, -Y, -Z);
	}

	const Vector4 Vector4::Zero = Vector4(0.0, 0.0, 0.0, 0.0);
	const Vector4 Vector4::One = Vector4(1.0, 1.0, 1.0, 1.0);

	Vector4::Vector4(double x, double y, double z, double w) noexcept :
		X(x), Y(y), Z(z), W(w)
	{}

	Vector4::Vector4(const Vector2 & vec2, double z, double w) noexcept : Vector4(vec2.X, vec2.Y, z, w)
	{}

	Vector4::Vector4(const Vector3& vec3, double w) noexcept : Vector4(vec3.X, vec3.Y, vec3.Z, w)
	{}

	double Vector4::Dot(const Vector4& a, const Vector4& b) noexcept
	{
		return a.Dot(b);
	}

	double Vector4::GetLengthSquared() const noexcept
	{
		return X * X + Y * Y + Z * Z + W * W;
	}

	double Vector4::GetLength() const noexcept
	{
		return Math::Sqrt(GetLengthSquared());
	}

	void Vector4::Normalize(bool safe) noexcept
	{
		if (safe && 
			Math::Approximately(X, 0.0) && 
			Math::Approximately(Y, 0.0) && 
			Math::Approximately(Z, 0.0) && 
			Math::Approximately(W, 0.0))
			return;

		const double length = GetLength();
		X /= length;
		Y /= length;
		Z /= length;
		W /= length;
	}

	Vector4 Vector4::Normalized(bool safe) const noexcept
	{
		Vector4 copy = *this;
		copy.Normalize(safe);
		return copy;
	}

	double Vector4::Dot(const Vector4& other) const noexcept
	{
		return X * other.X +
			Y * other.Y +
			Z * other.Z +
			W * other.W;
	}

	Vector4 Vector4::operator+(const Vector4 & other) const noexcept
	{
		return Vector4(X + other.X, Y + other.Y, Z + other.Z, W + other.W);
	}

	Vector4 Vector4::operator-(const Vector4& other) const noexcept
	{
		return Vector4(X - other.X, Y - other.Y, Z - other.Z, W - other.W);
	}

	Vector4 Vector4::operator*(double scalar) const noexcept
	{
		return Vector4(X * scalar, Y * scalar, Z * scalar, W * scalar);
	}

	Vector4 Vector4::operator*(const Vector4& other) const noexcept
	{
		return Vector4(X * other.X, Y * other.Y, Z * other.Z, W * other.W);
	}

	Vector4 Vector4::operator/(double divisor) const noexcept
	{
		return Vector4(X / divisor, Y / divisor, Z / divisor, W / divisor);
	}

	Vector4 Vector4::operator/(const Vector4& other) const noexcept
	{
		return Vector4(X / other.X, Y / other.Y, Z / other.Z, W / other.W);
	}
}