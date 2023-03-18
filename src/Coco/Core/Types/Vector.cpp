#include "Vector.h"

#include "Color.h"

namespace Coco
{
	const Vector2Int Vector2Int::Zero = Vector2Int(0, 0);
	const Vector2Int Vector2Int::One = Vector2Int(1, 1);

	Vector2Int::Vector2Int(int x, int y) noexcept : X(x), Y(y)
	{}

	Vector2Int::operator Vector2() const noexcept { return Vector2(X, Y); }

	const Vector2 Vector2::Zero = Vector2(0.0, 0.0);
	const Vector2 Vector2::One = Vector2(1.0, 1.0);

	const Vector2 Vector2::Right = Vector2(1.0, 0.0);
	const Vector2 Vector2::Left = Vector2(-1.0, 0.0);
	const Vector2 Vector2::Up = Vector2(0.0, 1.0);
	const Vector2 Vector2::Down = Vector2(0.0, -1.0);

	Vector2::Vector2(double x, double y) noexcept :
		X(x), Y(y)
	{}

	Vector2::Vector2(const Vector3 & vec) noexcept : Vector2(vec.X, vec.Y) {}

	double Vector2::DistanceBetween(const Vector2& a, const Vector2& b) noexcept
	{
		const Vector2 diff = a - b;
		return diff.GetLength();
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

	Vector2::operator Vector3() const noexcept { return Vector3(X, Y, 0.0); }

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

	Vector3::Vector3(const Vector2 & vec2, double z) noexcept : Vector3(vec2.X, vec2.Y, z) {}

	Vector3::Vector3(const Vector4 & vec4) noexcept : Vector3(vec4.X, vec4.Y, vec4.Z) {}

	double Vector3::DistanceBetween(const Vector3 & a, const Vector3 & b) noexcept
	{
		const Vector3 diff = a - b;
		return diff.GetLength();
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

	Vector3::operator Vector4() const noexcept { return Vector4(X, Y, Z, 0.0); }

	const Vector4 Vector4::Zero = Vector4(0.0, 0.0, 0.0, 0.0);
	const Vector4 Vector4::One = Vector4(1.0, 1.0, 1.0, 1.0);

	Vector4::Vector4(double x, double y, double z, double w) noexcept :
		X(x), Y(y), Z(z), W(w)
	{}

	Vector4::Vector4(const Vector2& vec2, double z, double w) noexcept : Vector4(vec2.X, vec2.Y, z, w) {}

	Vector4::Vector4(const Vector3& vec3, double w) noexcept : Vector4(vec3.X, vec3.Y, vec3.Z, w) {}

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

	Vector4::operator Color() const noexcept
	{
		return Color(X, Y, Z, W);
	}
}