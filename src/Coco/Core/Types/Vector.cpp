#include "Vector.h"

#include "Color.h"
#include "Array.h"

#include <span>

namespace Coco
{
	void ParseIntArray(const string& str, std::span<int> values)
	{
		uint64_t currentCharacterIndex = 0;
		uint64_t fieldIndex = 0;

		while (currentCharacterIndex < str.length())
		{
			if (fieldIndex >= values.size())
				break;

			uint64_t endIndex = str.find_first_of(',', currentCharacterIndex);

			if (endIndex == string::npos)
				endIndex = str.length();

			string part = str.substr(currentCharacterIndex, endIndex);
			values[fieldIndex] = atoi(part.c_str());

			currentCharacterIndex = endIndex + 1;
			fieldIndex++;
		}

	}
	void ParseDoubleArray(const string& str, std::span<double> values)
	{
		uint64_t currentCharacterIndex = 0;
		uint64_t fieldIndex = 0;

		while (currentCharacterIndex < str.length())
		{
			if (fieldIndex >= values.size())
				break;

			uint64_t endIndex = str.find_first_of(',', currentCharacterIndex);

			if (endIndex == string::npos)
				endIndex = str.length();

			string part = str.substr(currentCharacterIndex, endIndex - currentCharacterIndex);
			values[fieldIndex] = atof(part.c_str());

			currentCharacterIndex = endIndex + 1;
			fieldIndex++;
		}
	}

	const Vector2Int Vector2Int::Zero = Vector2Int(0, 0);
	const Vector2Int Vector2Int::One = Vector2Int(1, 1);

	Vector2Int::Vector2Int(int x, int y) noexcept : X(x), Y(y)
	{}

	Vector2Int Vector2Int::Parse(const string& str)
	{
		Array<int, 2> values = { 0 };

		ParseIntArray(str, std::span<int>{values});

		return Vector2Int(values[0], values[1]);
	}

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

	Vector2 Vector2::Parse(const string& str)
	{
		Array<double, 2> values = { 0.0 };

		ParseDoubleArray(str, std::span<double>{values});

		return Vector2(values[0], values[1]);
	}

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

	Vector3 Vector3::Parse(const string& str)
	{
		Array<double, 3> values = { 0.0 };

		ParseDoubleArray(str, std::span<double>{values});

		return Vector3(values[0], values[1], values[2]);
	}

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

	Vector4 Vector4::Parse(const string& str)
	{
		Array<double, 4> values = {0.0};

		ParseDoubleArray(str, std::span<double>{values});

		return Vector4(values[0], values[1], values[2], values[3]);
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

	Vector4::operator Color() const noexcept
	{
		return Color(X, Y, Z, W);
	}
}