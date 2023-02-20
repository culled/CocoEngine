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

		SizeInt();
		SizeInt(int width, int height);

		SizeInt operator+(const SizeInt& other) const;
		SizeInt operator-(const SizeInt& other) const;
		bool operator==(const SizeInt& other) const;
		bool operator!=(const SizeInt& other) const;
	};
}