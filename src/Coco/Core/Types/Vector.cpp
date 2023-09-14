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

	Vector2::Vector2() : Vector2(0, 0)
	{}

	Vector2::Vector2(double x, double y) :
		X(x),
		Y(y)
	{}

	double Vector2::DistanceBetween(const Vector2& p0, const Vector2& p1)
	{
		return (p0 - p1).GetLength();
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
	
	Vector2Int::Vector2Int() : Vector2Int(0, 0)
	{}

	Vector2Int::Vector2Int(int x, int y) : 
		X(x),
		Y(y)
	{}

	double Vector2Int::DistanceBetween(const Vector2Int& p0, const Vector2Int& p1)
	{
		return (p0 - p1).GetLength();
	}

	string Vector2Int::ToString() const
	{
		return FormatString("{}, {}", X, Y);
	}
}