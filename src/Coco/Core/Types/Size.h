#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Represents a width and height using integers
	/// </summary>
	struct COCOAPI SizeInt
	{
		int Width, Height;

		SizeInt(int width, int height) : Width(width), Height(height) {}
		SizeInt() : SizeInt(0, 0) {}
	};
}