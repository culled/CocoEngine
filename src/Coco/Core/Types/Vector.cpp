#include "Vector.h"

namespace Coco
{
	const Vector2Int Vector2Int::Zero = Vector2Int(0, 0);

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

	Vector2Int Vector2Int::operator*(const int& scalar) const
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
}