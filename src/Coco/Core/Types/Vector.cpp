#include "Corepch.h"
#include "Vector.h"

namespace Coco
{
	const Vector2 Vector2::Zero = Vector2(0, 0);
	const Vector2 Vector2::One = Vector2(1, 1);
	const Vector2 Vector2::Right = Vector2(1, 0);
	const Vector2 Vector2::Left = Vector2(-1, 0);
	const Vector2 Vector2::Up = Vector2(0, 1);
	const Vector2 Vector2::Down = Vector2(0, -1);

	Vector2::Vector2() : 
		Vector2(0, 0)
	{}

	Vector2::Vector2(double x, double y) :
		X(x),
		Y(y)
	{}

	void Vector2::Normalize(bool safe)
	{
		const double length = GetLength();

		if (safe && Math::Equal(length, 0.0))
			return;

		X /= length;
		Y /= length;
	}

	Vector2 Vector2::Normalized(bool safe) const
	{
		Vector2 copy(*this);
		copy.Normalize(safe);
		return copy;
	}

	Vector2 Vector2::Project(const Vector2& normal) const
	{
		// https://www.youtube.com/watch?v=naaeH1qbjdQ
		double VdotN = Dot(normal);
		double NdotN = normal.Dot(normal);

		return normal * (VdotN / NdotN);
	}

	Vector2 Vector2::Reflect(const Vector2& normal) const
	{
		// https://www.youtube.com/watch?v=naaeH1qbjdQ
		Vector2 projection = Project(normal);

		return *this - projection * 2.0;
	}

	Vector2 Vector2::Refract(const Vector2& normal, double ior) const
	{
		// https://stackoverflow.com/questions/29758545/how-to-find-refraction-vector-from-incoming-vector-and-surface-normal
		double cosI = Math::Clamp(-Dot(normal), 0.0, 1.0);
		const double sinT2 = Math::Pow(ior, 2.0) * (1.0 - Math::Pow(cosI, 2.0));

		if (sinT2 > 1.0)
			return Vector2::Zero;

		const double cosT = Math::Sqrt(1.0 - sinT2);
		return *this * ior + normal * (ior * cosI - cosT);
	}

	string Vector2::ToString() const
	{
		return FormatString("{}, {}", X, Y);
	}

	const Vector2Int Vector2Int::Zero = Vector2Int(0, 0);
	const Vector2Int Vector2Int::One = Vector2Int(1, 1);
	const Vector2Int Vector2Int::Right = Vector2Int(1, 0);
	const Vector2Int Vector2Int::Left = Vector2Int(-1, 0);
	const Vector2Int Vector2Int::Up = Vector2Int(0, 1);
	const Vector2Int Vector2Int::Down = Vector2Int(0, -1);
	
	Vector2Int::Vector2Int() : 
		Vector2Int(0, 0)
	{}

	Vector2Int::Vector2Int(int x, int y) : 
		X(x),
		Y(y)
	{}

	string Vector2Int::ToString() const
	{
		return FormatString("{}, {}", X, Y);
	}

	const Vector3 Vector3::Zero = Vector3(0, 0, 0);
	const Vector3 Vector3::One = Vector3(1, 1, 1);
	const Vector3 Vector3::Right = Vector3(1, 0, 0);
	const Vector3 Vector3::Left = Vector3(-1, 0, 0);
	const Vector3 Vector3::Up = Vector3(0, 1, 0);
	const Vector3 Vector3::Down = Vector3(0, -1, 0);
	const Vector3 Vector3::Forward = Vector3(0, 0, 1);
	const Vector3 Vector3::Backward = Vector3(0, 0, -1);

	Vector3::Vector3() : 
		Vector3(0, 0, 0)
	{}

	Vector3::Vector3(double x, double y, double z) :
		X(x),
		Y(y),
		Z(z)
	{}

	Vector3::Vector3(const Vector2& vec2, double z) : 
		Vector3(vec2.X, vec2.Y, 0)
	{}

	void Vector3::Normalize(bool safe)
	{
		const double length = GetLength();

		if (safe && Math::Equal(length, 0.0))
			return;

		X /= length;
		Y /= length;
		Z /= length;
	}

	Vector3 Vector3::Normalized(bool safe) const
	{
		Vector3 copy(*this);
		copy.Normalize(safe);
		return copy;
	}

	Vector3 Vector3::Project(const Vector3& normal) const
	{
		// https://www.youtube.com/watch?v=naaeH1qbjdQ
		double VdotN = Dot(normal);
		double NdotN = normal.Dot(normal);

		return normal * (VdotN / NdotN);
	}

	Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		// https://www.youtube.com/watch?v=naaeH1qbjdQ
		Vector3 projection = Project(normal);

		return *this - projection * 2.0;
	}

	Vector3 Vector3::Refract(const Vector3& normal, double ior) const
	{
		// https://stackoverflow.com/questions/29758545/how-to-find-refraction-vector-from-incoming-vector-and-surface-normal
		double cosI = Math::Clamp(-Dot(normal), 0.0, 1.0);
		const double sinT2 = Math::Pow(ior, 2.0) * (1.0 - Math::Pow(cosI, 2.0));

		if (sinT2 > 1.0)
			return Vector3::Zero;

		const double cosT = Math::Sqrt(1.0 - sinT2);
		return *this * ior + normal * (ior * cosI - cosT);
	}

	string Vector3::ToString() const
	{
		return FormatString("{}, {}, {}", X, Y, Z);
	}

	const Vector3Int Vector3Int::Zero = Vector3Int(0, 0, 0);
	const Vector3Int Vector3Int::One = Vector3Int(1, 1, 1);
	const Vector3Int Vector3Int::Right = Vector3Int(1, 0, 0);
	const Vector3Int Vector3Int::Left = Vector3Int(-1, 0, 0);
	const Vector3Int Vector3Int::Up = Vector3Int(0, 1, 0);
	const Vector3Int Vector3Int::Down = Vector3Int(0, -1, 0);
	const Vector3Int Vector3Int::Forward = Vector3Int(0, 0, 1);
	const Vector3Int Vector3Int::Backward = Vector3Int(0, 0, -1);

	Vector3Int::Vector3Int() :
		Vector3Int(0, 0, 0)
	{}

	Vector3Int::Vector3Int(int x, int y, int z) :
		X(x),
		Y(y),
		Z(z)
	{}

	string Vector3Int::ToString() const
	{
		return FormatString("{}, {}, {}", X, Y, Z);
	}

	const Vector4 Vector4::Zero = Vector4(0, 0, 0, 0);
	const Vector4 Vector4::One = Vector4(1, 1, 1, 1);

	Vector4::Vector4() : 
		Vector4(0, 0, 0, 0)
	{}

	Vector4::Vector4(double x, double y, double z, double w) :
		X(x),
		Y(y),
		Z(z),
		W(w)
	{}

	Vector4::Vector4(const Vector2& vec2, double z, double w) : 
		Vector4(vec2.X, vec2.Y, 0, 0)
	{}

	Vector4::Vector4(const Vector3& vec3, double w) :
		Vector4(vec3.X, vec3.Y, vec3.Z, w)
	{}

	double Vector4::DistanceBetween(const Vector4& p0, const Vector4& p1)
	{
		return (p0 - p1).GetLength();
	}

	void Vector4::Normalize(bool safe)
	{
		const double length = GetLength();

		if (safe && Math::Equal(length, 0.0))
			return;

		X /= length;
		Y /= length;
		Z /= length;
		W /= length;
	}

	Vector4 Vector4::Normalized(bool safe) const
	{
		Vector4 copy(*this);
		copy.Normalize(safe);
		return copy;
	}

	string Vector4::ToString() const
	{
		return FormatString("{}, {}, {}, {}", X, Y, Z, W);
	}

	const Vector4Int Vector4Int::Zero = Vector4Int(0, 0, 0, 0);
	const Vector4Int Vector4Int::One = Vector4Int(1, 1, 1, 1);

	Vector4Int::Vector4Int() :
		Vector4Int(0, 0, 0, 0)
	{}

	Vector4Int::Vector4Int(int x, int y, int z, int w) :
		X(x),
		Y(y),
		Z(z),
		W(w)
	{}

	string Vector4Int::ToString() const
	{
		return FormatString("{}, {}, {}, {}", X, Y, Z, W);
	}
}