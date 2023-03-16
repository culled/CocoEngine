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

		SizeInt() noexcept = default;
		SizeInt(int width, int height) noexcept;
		virtual ~SizeInt() = default;

		SizeInt operator+(const SizeInt& other) const noexcept { return SizeInt(Width + other.Width, Height + other.Height); }
		void operator+=(const SizeInt& other) noexcept { Width += other.Width; Height += other.Height; }

		SizeInt operator-(const SizeInt& other) const noexcept { return SizeInt(Width - other.Width, Height - other.Height); }
		void operator-=(const SizeInt& other) noexcept { Width -= other.Width; Height -= other.Height; }

		SizeInt operator*(const SizeInt& other) const noexcept { return SizeInt(Width * other.Width, Height * other.Height); }
		void operator*=(const SizeInt& other) noexcept { Width *= other.Width; Height *= other.Height; }

		SizeInt operator*(int scalar) const noexcept { return SizeInt(Width * scalar, Height * scalar); }
		void operator*=(int scalar) noexcept { Width *= scalar; Height *= scalar; }

		SizeInt operator/(const SizeInt& other) const noexcept { return SizeInt(Width / other.Width, Height / other.Height); }
		void operator/=(const SizeInt& other) noexcept { Width /= other.Width; Height /= other.Height; }

		SizeInt operator/(int scalar) const noexcept { return SizeInt(Width / scalar, Height / scalar); }
		void operator/=(int scalar) noexcept { Width /= scalar; Height /= scalar; }

		bool operator==(const SizeInt& other) const noexcept { return Width == other.Width && Height == other.Height; }
		bool operator!=(const SizeInt& other) const noexcept { return Width != other.Width || Height != other.Height; }
	};
}