#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Represents a width and height using integers
	/// </summary>
	struct COCOAPI SizeInt
	{
		int Width = 0;
		int Height = 0;

		SizeInt() = default;
		SizeInt(int width, int height) noexcept;

		SizeInt operator+(const SizeInt& other) const noexcept;
		SizeInt operator-(const SizeInt& other) const noexcept;
		bool operator==(const SizeInt& other) const noexcept;
		bool operator!=(const SizeInt& other) const noexcept;
	};
}