#pragma once

#include <Coco/Core/API.h>

#include <Coco/Core/Math/Math.h>

namespace Coco
{
	struct Vector2Int;
	struct Vector2;

	/// <summary>
	/// A size with a width and height using integers
	/// </summary>
	struct COCOAPI SizeInt
	{
		/// <summary>
		/// A zero size
		/// </summary>
		static const SizeInt Zero;

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

		SizeInt operator/(int divisor) const noexcept { return SizeInt(Width / divisor, Height / divisor); }
		void operator/=(int divisor) noexcept { Width /= divisor; Height /= divisor; }

		bool operator==(const SizeInt& other) const noexcept { return Width == other.Width && Height == other.Height; }
		bool operator!=(const SizeInt& other) const noexcept { return Width != other.Width || Height != other.Height; }

		operator Vector2Int() const noexcept;
		operator Vector2() const noexcept;
	};

	/// <summary>
	/// A size with a width and height using decimals
	/// </summary>
	struct COCOAPI Size
	{
		/// <summary>
		/// A zero size
		/// </summary>
		static const Size Zero;

		double Width = 0;
		double Height = 0;

		Size() noexcept = default;
		Size(double width, double height) noexcept;
		virtual ~Size() = default;

		/// <summary>
		/// Compares if this size equals another size
		/// </summary>
		/// <param name="other">The other size</param>
		/// <param name="tolerance">The difference tolerance</param>
		/// <returns>True if the two sizes are within the tolerance of each other</returns>
		bool Equals(const Size& other, double tolerance = Math::Epsilon) const noexcept
		{
			return Math::Approximately(Width, other.Width, tolerance) && Math::Approximately(Height, other.Height, tolerance);
		}

		Size operator+(const Size& other) const noexcept { return Size(Width + other.Width, Height + other.Height); }
		void operator+=(const Size& other) noexcept { Width += other.Width; Height += other.Height; }

		Size operator-(const Size& other) const noexcept { return Size(Width - other.Width, Height - other.Height); }
		void operator-=(const Size& other) noexcept { Width -= other.Width; Height -= other.Height; }

		Size operator*(const Size& other) const noexcept { return Size(Width * other.Width, Height * other.Height); }
		void operator*=(const Size& other) noexcept { Width *= other.Width; Height *= other.Height; }

		Size operator*(double scalar) const noexcept { return Size(Width * scalar, Height * scalar); }
		void operator*=(double scalar) noexcept { Width *= scalar; Height *= scalar; }

		Size operator/(const Size& other) const noexcept { return Size(Width / other.Width, Height / other.Height); }
		void operator/=(const Size& other) noexcept { Width /= other.Width; Height /= other.Height; }

		Size operator/(double divisor) const noexcept { return Size(Width / divisor, Height / divisor); }
		void operator/=(double divisor) noexcept { Width /= divisor; Height /= divisor; }

		bool operator==(const Size& other) const noexcept { return Equals(other); }
		bool operator!=(const Size& other) const noexcept { return !Equals(other); }

		operator Vector2() const noexcept;
	};
}