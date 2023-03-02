#include "Vector.h"
#include <Coco/Core/Math/Math.h>

namespace Coco
{
	const Vector2Int Vector2Int::Zero = Vector2Int(0, 0);
	const Vector2Int Vector2Int::One = Vector2Int(1, 1);

	Vector2Int::Vector2Int() : Vector2Int(0, 0)
	{}

	Vector2Int::Vector2Int(int x, int y) : X(x), Y(y)
	{}

	Vector2Int Vector2Int::operator+(const Vector2Int& other) const
	{
		return Vector2Int(this->X + other.X, this->Y + other.Y);
	}

	Vector2Int Vector2Int::operator-(const Vector2Int& other) const
	{
		return Vector2Int(this->X - other.X, this->Y - other.Y);
	}

	Vector2Int Vector2Int::operator*(int scalar) const
	{
		return Vector2Int(this->X * scalar, this->Y * scalar);
	}

	bool Vector2Int::operator==(const Vector2Int& other) const
	{
		return this->X == other.X && this->Y == other.Y;
	}

	bool Vector2Int::operator!= (const Vector2Int& other) const
	{
		return !(*this == other);
	}

	const Vector2 Vector2::Zero = Vector2(0.0, 0.0);
	const Vector2 Vector2::One = Vector2(1.0, 1.0);

	const Vector2 Vector2::Right = Vector2(1.0, 0.0);
	const Vector2 Vector2::Left = Vector2(-1.0, 0.0);
	const Vector2 Vector2::Up = Vector2(0.0, 1.0);
	const Vector2 Vector2::Down = Vector2(0.0, -1.0);

	Vector2::Vector2() : Vector2(0, 0)
	{}

	Vector2::Vector2(double x, double y) :
		X(x), Y(y)
	{}

	double Vector2::DistanceBetween(const Vector2& a, const Vector2& b)
	{
		const Vector2 diff = a - b;
		return diff.GetLength();
	}

	double Vector2::Dot(const Vector2& a, const Vector2& b)
	{
		return a.Dot(b);
	}

	double Vector2::GetLengthSquared() const
	{
		return this->X * this->X + this->Y * this->Y;
	}

	double Vector2::GetLength() const
	{
		return Math::Sqrt(GetLengthSquared());
	}

	void Vector2::Normalize(bool safe)
	{
		if (safe && 
			Math::Approximately(this->X, 0.0) && 
			Math::Approximately(this->Y, 0.0))
			return;

		const double length = GetLength();
		X /= length;
		Y /= length;
	}

	Vector2 Vector2::Normalized(bool safe) const
	{
		Vector2 copy = *this;
		copy.Normalize(safe);
		return copy;
	}

	bool Vector2::Equals(const Vector2& other, double tolerance) const
	{
		if (!Math::Approximately(this->X, other.X, tolerance))
			return false;

		if (!Math::Approximately(this->Y, other.Y, tolerance))
			return false;

		return true;
	}

	double Vector2::Dot(const Vector2& other) const
	{
		double p = 0.0;
		p += this->X * other.X;
		p += this->Y * other.Y;
		return p;
	}

	Vector2 Vector2::operator+(const Vector2 & other) const
	{
		return Vector2(this->X + other.X, this->Y + other.Y);
	}

	Vector2 Vector2::operator-(const Vector2& other) const
	{
		return Vector2(this->X - other.X, this->Y - other.Y);
	}

	Vector2 Vector2::operator*(double scalar) const
	{
		return Vector2(this->X * scalar, this->Y * scalar);
	}

	Vector2 Vector2::operator*(const Vector2& other) const
	{
		return Vector2(this->X * other.X, this->Y * other.Y);
	}

	Vector2 Vector2::operator/(double divisor) const
	{
		return Vector2(this->X / divisor, this->Y / divisor);
	}

	Vector2 Vector2::operator/(const Vector2& other) const
	{
		return Vector2(this->X / other.X, this->Y / other.Y);
	}

	const Vector3 Vector3::Zero = Vector3(0.0, 0.0, 0.0);
	const Vector3 Vector3::One = Vector3(1.0, 1.0, 1.0);

	const Vector3 Vector3::Right = Vector3(1.0, 0.0, 0.0);
	const Vector3 Vector3::Left = Vector3(-1.0, 0.0, 0.0);
	const Vector3 Vector3::Up = Vector3(0.0, 1.0, 0.0);
	const Vector3 Vector3::Down = Vector3(0.0, -1.0, 0.0);
	const Vector3 Vector3::Backwards = Vector3(0.0, 0.0, 1.0);
	const Vector3 Vector3::Forwards = Vector3(0.0, 0.0, -1.0);

	Vector3::Vector3() : Vector3(0, 0 ,0)
	{}

	Vector3::Vector3(double x, double y, double z) : 
		X(x), Y(y), Z(z)
	{}

	Vector3::Vector3(const Vector2& vec2, double z): Vector3(vec2.X, vec2.Y, z)
	{}

	Vector3::Vector3(const Vector4& vec4) : Vector3(vec4.X, vec4.Y, vec4.Z)
	{}

	double Vector3::DistanceBetween(const Vector3 & a, const Vector3 & b)
	{
		const Vector3 diff = a - b;
		return diff.GetLength();
	}

	double Vector3::Dot(const Vector3& a, const Vector3& b)
	{
		return a.Dot(b);
	}

	Vector3 Vector3::Cross(const Vector3& a, const Vector3& b)
	{
		return a.Cross(b);
	}

	double Vector3::GetLengthSquared() const
	{
		return this->X * this->X + this->Y * this->Y + this->Z * this->Z;
	}

	double Vector3::GetLength() const
	{
		return Math::Sqrt(GetLengthSquared());
	}

	void Vector3::Normalize(bool safe)
	{
		if (safe && 
			Math::Approximately(this->X, 0.0) && 
			Math::Approximately(this->Y, 0.0) && 
			Math::Approximately(this->Z, 0.0))
			return;

		const double length = GetLength();
		X /= length;
		Y /= length;
		Z /= length;
	}

	Vector3 Vector3::Normalized(bool safe) const
	{
		Vector3 copy = *this;
		copy.Normalize();
		return copy;
	}

	bool Vector3::Equals(const Vector3& other, double tolerance) const
	{
		if(!Math::Approximately(this->X, other.X, tolerance))
			return false;

		if (!Math::Approximately(this->Y, other.Y, tolerance))
			return false;

		if (!Math::Approximately(this->Z, other.Z, tolerance))
			return false;

		return true;
	}

	double Vector3::Dot(const Vector3& other) const
	{
		double p = 0.0;
		p += this->X * other.X;
		p += this->Y * other.Y;
		p += this->Z * other.Z;
		return p;
	}

	Vector3 Vector3::Cross(const Vector3& other) const
	{
		return Vector3(
			this->Y * other.Z - this->Z * other.Y, 
			this->Z * other.X - this->X * other.Z, 
			this->X * other.Y - this->Y * other.X);
	}

	Vector3 Vector3::operator+(const Vector3 & other) const
	{
		return Vector3(this->X + other.X, this->Y + other.Y, this->Z + other.Z);
	}

	Vector3 Vector3::operator-(const Vector3& other) const
	{
		return Vector3(this->X - other.X, this->Y - other.Y, this->Z - other.Z);
	}

	Vector3 Vector3::operator*(double scalar) const
	{
		return Vector3(this->X * scalar, this->Y * scalar, this->Z * scalar);
	}

	Vector3 Vector3::operator*(const Vector3& other) const
	{
		return Vector3(this->X * other.X, this->Y * other.Y, this->Z * other.Z);
	}

	Vector3 Vector3::operator/(double divisor) const
	{
		return Vector3(this->X / divisor, this->Y / divisor, this->Z / divisor);
	}

	Vector3 Vector3::operator/(const Vector3& other) const
	{
		return Vector3(this->X / other.X, this->Y / other.Y, this->Z / other.Z);
	}

	Vector3 Vector3::operator-() const
	{
		return Vector3(-this->X, -this->Y, -this->Z);
	}

	const Vector4 Vector4::Zero = Vector4(0.0, 0.0, 0.0, 0.0);
	const Vector4 Vector4::One = Vector4(1.0, 1.0, 1.0, 1.0);

	Vector4::Vector4() : Vector4(0, 0, 0, 0)
	{}

	Vector4::Vector4(double x, double y, double z, double w) : 
		X(x), Y(y), Z(z), W(w)
	{}

	Vector4::Vector4(const Vector2 & vec2, double z, double w) : Vector4(vec2.X, vec2.Y, z, w)
	{}

	Vector4::Vector4(const Vector3& vec3, double w) : Vector4(vec3.X, vec3.Y, vec3.Z, w)
	{}

	double Vector4::Dot(const Vector4& a, const Vector4& b)
	{
		return a.Dot(b);
	}

	double Vector4::GetLengthSquared() const
	{
		return this->X * this->X + this->Y * this->Y + this->Z * this->Z + this->W * this->W;
	}

	double Vector4::GetLength() const
	{
		return Math::Sqrt(GetLengthSquared());
	}

	void Vector4::Normalize(bool safe)
	{
		if (safe && 
			Math::Approximately(this->X, 0.0) && 
			Math::Approximately(this->Y, 0.0) && 
			Math::Approximately(this->Z, 0.0) && 
			Math::Approximately(this->W, 0.0))
			return;

		const double length = GetLength();
		this->X /= length;
		this->Y /= length;
		this->Z /= length;
		this->W /= length;
	}

	Vector4 Vector4::Normalized(bool safe) const
	{
		Vector4 copy = *this;
		copy.Normalize();
		return copy;
	}

	double Vector4::Dot(const Vector4& other) const
	{
		double p = 0.0;
		p += this->X * other.X;
		p += this->Y * other.Y;
		p += this->Z * other.Z;
		p += this->W * other.W;
		return p;
	}

	Vector4 Vector4::operator+(const Vector4 & other) const
	{
		return Vector4(this->X + other.X, this->Y + other.Y, this->Z + other.Z, this->W + other.W);
	}

	Vector4 Vector4::operator-(const Vector4& other) const
	{
		return Vector4(this->X - other.X, this->Y - other.Y, this->Z - other.Z, this->W - other.W);
	}

	Vector4 Vector4::operator*(double scalar) const
	{
		return Vector4(this->X * scalar, this->Y * scalar, this->Z * scalar, this->W * scalar);
	}

	Vector4 Vector4::operator*(const Vector4& other) const
	{
		return Vector4(this->X * other.X, this->Y * other.Y, this->Z * other.Z, this->W * other.W);
	}

	Vector4 Vector4::operator/(double divisor) const
	{
		return Vector4(this->X / divisor, this->Y / divisor, this->Z / divisor, this->W / divisor);
	}

	Vector4 Vector4::operator/(const Vector4& other) const
	{
		return Vector4(this->X / other.X, this->Y / other.Y, this->Z / other.Z, this->W / other.W);
	}
}