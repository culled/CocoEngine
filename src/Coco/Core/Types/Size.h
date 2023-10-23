#pragma once

#include "String.h"
#include "../Math/Math.h"

namespace Coco
{
	struct Vector2;
	struct Vector2Int;

	/// @brief A size that is backed by double values
	struct Size
	{
		/// @brief A size with zero width and height
		static const Size Zero;

		/// @brief The width
		double Width;

		/// @brief The height
		double Height;

		Size();
		Size(double width, double height);

		constexpr void operator+=(const Size& other) { Width += other.Width; Height += other.Height; }
		constexpr void operator-=(const Size& other) { Width -= other.Width; Height -= other.Height; }

		Size operator+(const Size& other) const { return Size(Width + other.Width, Height + other.Height); }
		Size operator-(const Size& other) const { return Size(Width - other.Width, Height - other.Height); }

		constexpr void operator*=(double scalar) { Width *= scalar; Height *= scalar; }
		constexpr void operator/=(double divisor) { Width /= divisor; Height /= divisor; }

		Size operator*(double scalar) const { return Size(Width * scalar, Height * scalar); }
		Size operator/(double divisor) const { return Size(Width / divisor, Height / divisor); }

		operator Vector2() const;

		/// @brief Determines if this size equals another
		/// @param other The other size
		/// @param threshold The threshold used to determine equality
		/// @return True if the given size equals this one
		constexpr bool Equals(const Size& other, double threshold = Math::Epsilon) const
		{
			return Math::Approximately(Width, other.Width, threshold) &&
				Math::Approximately(Height, other.Height, threshold);
		}

		/// @brief Gets a string representation of this size
		/// @return The string representation
		string ToString() const;
	};

	/// @brief A size that is backed by int values
	struct SizeInt
	{
		/// @brief A size with zero width and height
		static const SizeInt Zero;

		/// @brief The width
		int Width;

		/// @brief The height
		int Height;

		SizeInt();
		SizeInt(int width, int height);

		constexpr bool operator==(const SizeInt& other) const { return Width == other.Width && Height == other.Height; }

		constexpr void operator+=(const SizeInt& other) { Width += other.Width; Height += other.Height; }
		constexpr void operator-=(const SizeInt& other) { Width -= other.Width; Height -= other.Height; }

		SizeInt operator+(const SizeInt& other) const { return SizeInt(Width + other.Width, Height + other.Height); }
		SizeInt operator-(const SizeInt& other) const { return SizeInt(Width - other.Width, Height - other.Height); }

		constexpr void operator*=(int scalar) { Width *= scalar; Height *= scalar; }
		constexpr void operator/=(int divisor) { Width /= divisor; Height /= divisor; }

		SizeInt operator*(int scalar) const { return SizeInt(Width * scalar, Height * scalar); }
		SizeInt operator/(int divisor) const { return SizeInt(Width / divisor, Height / divisor); }

		operator Size() const { return Size(Width, Height); }

		operator Vector2Int() const;

		/// @brief Gets a string representation of this size
		/// @return The string representation
		string ToString() const;
	};
}