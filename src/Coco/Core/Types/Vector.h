#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Represents a 2D vector using integers
	/// </summary>
	struct COCOAPI Vector2Int
	{
		static const Vector2Int Zero;

		int X, Y;

		Vector2Int(int x, int y) : X(x), Y(y) {}
		Vector2Int() : Vector2Int(0, 0) {}

		Vector2Int operator+(const Vector2Int& other) const
		{
			return Vector2Int(this->X + other.X, this->Y + other.Y);
		}

		Vector2Int operator-(const Vector2Int& other) const
		{
			return Vector2Int(this->X - other.X, this->Y - other.Y);
		}

		Vector2Int operator*(const int& scalar) const
		{
			return Vector2Int(this->X * scalar, this->Y * scalar);
		}

		bool operator==(const Vector2Int& other) const
		{
			return this->X == other.X && this->Y == other.Y;
		}

		bool operator!= (const Vector2Int& other) const
		{
			return !(*this == other);
		}
	};
}