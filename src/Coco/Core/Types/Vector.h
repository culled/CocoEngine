#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Represents a 2D vector using integers
	/// </summary>
	struct COCOAPI Vector2Int
	{
		int X, Y;

		Vector2Int(int x, int y) : X(x), Y(y) {}
		Vector2Int() : Vector2Int(0, 0) {}
	};
}