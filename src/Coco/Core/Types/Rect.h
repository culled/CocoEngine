#pragma once

#include <Coco/Core/Core.h>

#include "Size.h"
#include "Vector.h"

namespace Coco
{
	/// <summary>
	/// A rectangle that uses integer coordinates
	/// </summary>
	struct COCOAPI RectInt
	{
		/// <summary>
		/// The size of the rectangle
		/// </summary>
		SizeInt Size;

		/// <summary>
		/// The bottom-left position of the rectangle
		/// </summary>
		Vector2Int Offset;

		RectInt() noexcept;
		RectInt(const Vector2Int& offset, const SizeInt& size) noexcept;
		RectInt(const Vector2Int& p0, const Vector2Int& p1) noexcept;

		/// <summary>
		/// Gets the bottom-left corner of this rectangle
		/// </summary>
		/// <returns>The bottom-left corner of this rectangle</returns>
		Vector2Int GetMin() const noexcept { return Offset; }

		/// <summary>
		/// Gets the top-right corner of this rectangle
		/// </summary>
		/// <returns>The top-right corner of this rectangle</returns>
		Vector2Int GetMax() const noexcept { return Offset + Size; }

		/// <summary>
		/// Gets the center of this rectangle
		/// </summary>
		/// <returns>The center of this rectangle</returns>
		Vector2 GetCenter() const noexcept { return static_cast<Vector2>(Offset) + Vector2(Size.Width / 2.0, Size.Height / 2.0); }

		/// <summary>
		/// Gets the position of the left edge of this rectangle
		/// </summary>
		/// <returns>The position of the left edge of this rectangle</returns>
		constexpr int GetLeft() const noexcept { return Offset.X; }

		/// <summary>
		/// Gets the position of the right edge of this rectangle
		/// </summary>
		/// <returns>The position of the right edge of this rectangle</returns>
		constexpr int GetRight() const noexcept { return Offset.X + Size.Width; }

		/// <summary>
		/// Gets the position of the bottom edge of this rectangle
		/// </summary>
		/// <returns>The position of the bottom edge of this rectangle</returns>
		constexpr int GetBottom() const noexcept { return Offset.Y; }

		/// <summary>
		/// Gets the position of the top edge of this rectangle
		/// </summary>
		/// <returns>The position of the top edge of this rectangle</returns>
		constexpr int GetTop() const noexcept { return Offset.Y + Size.Height; }

		/// <summary>
		/// Expands this rectangle to encompass the given point
		/// </summary>
		/// <param name="point">The point for this rectangle to cover</param>
		void Expand(const Vector2Int& point) noexcept;

		/// <summary>
		/// Tests if this rectangle intersects with another rectangle
		/// </summary>
		/// <param name="other">The other rectangle</param>
		/// <returns>True if there is an intersection</returns>
		bool Intersects(const RectInt& other) noexcept;

		/// <summary>
		/// Tests if this rectangle intersects with a point
		/// </summary>
		/// <param name="point">The point</param>
		/// <returns>True if there is an intersection</returns>
		bool Intersects(const Vector2Int& point) noexcept;
	};

	/// <summary>
	/// A rectangle that uses decinal coordinates
	/// </summary>
	struct COCOAPI Rect
	{
		/// <summary>
		/// The size of the rectangle
		/// </summary>
		Size Size;

		/// <summary>
		/// The bottom-left position of the rectangle
		/// </summary>
		Vector2 Offset;

		Rect() noexcept;
		Rect(const Vector2& offset, const Coco::Size& size) noexcept;
		Rect(const Vector2& p0, const Vector2& p1) noexcept;

		/// <summary>
		/// Gets the bottom-left corner of this rectangle
		/// </summary>
		/// <returns>The bottom-left corner of this rectangle</returns>
		Vector2 GetMin() const noexcept { return Offset; }

		/// <summary>
		/// Gets the top-right corner of this rectangle
		/// </summary>
		/// <returns>The top-right corner of this rectangle</returns>
		Vector2 GetMax() const noexcept { return Offset + Size; }

		/// <summary>
		/// Gets the center of this rectangle
		/// </summary>
		/// <returns>The center of this rectangle</returns>
		Vector2 GetCenter() const noexcept { return Offset + Vector2(Size.Width / 2.0, Size.Height / 2.0); }

		/// <summary>
		/// Gets the position of the left edge of this rectangle
		/// </summary>
		/// <returns>The position of the left edge of this rectangle</returns>
		constexpr double GetLeft() const noexcept { return Offset.X; }

		/// <summary>
		/// Gets the position of the right edge of this rectangle
		/// </summary>
		/// <returns>The position of the right edge of this rectangle</returns>
		constexpr double GetRight() const noexcept { return Offset.X + Size.Width; }

		/// <summary>
		/// Gets the position of the bottom edge of this rectangle
		/// </summary>
		/// <returns>The position of the bottom edge of this rectangle</returns>
		constexpr double GetBottom() const noexcept { return Offset.Y; }

		/// <summary>
		/// Gets the position of the top edge of this rectangle
		/// </summary>
		/// <returns>The position of the top edge of this rectangle</returns>
		constexpr double GetTop() const noexcept { return Offset.Y + Size.Height; }

		/// <summary>
		/// Expands this rectangle to encompass the given point
		/// </summary>
		/// <param name="point">The point for this rectangle to cover</param>
		void Expand(const Vector2& point) noexcept;

		/// <summary>
		/// Tests if this rectangle intersects with another rectangle
		/// </summary>
		/// <param name="other">The other rectangle</param>
		/// <returns>True if there is an intersection</returns>
		bool Intersects(const Rect& other) noexcept;

		/// <summary>
		/// Tests if this rectangle intersects with a point
		/// </summary>
		/// <param name="point">The point</param>
		/// <returns>True if there is an intersection</returns>
		bool Intersects(const Vector2& point) noexcept;
	};
}