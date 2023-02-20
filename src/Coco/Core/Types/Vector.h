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

		Vector2Int();
		Vector2Int(int x, int y);

		Vector2Int operator+(const Vector2Int& other) const;
		Vector2Int operator-(const Vector2Int& other) const;
		Vector2Int operator*(const int& scalar) const;
		bool operator==(const Vector2Int& other) const;
		bool operator!= (const Vector2Int& other) const;
	};
}