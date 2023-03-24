#pragma once

#include <Coco/Core/API.h>

#include "Size.h"
#include "Vector.h"

namespace Coco
{
	/// @brief A rectangle that uses integer coordinates
	struct COCOAPI RectInt
	{
		/// @brief The size of the rectangle
		SizeInt Size;

		/// @brief The bottom-left position of the rectangle
		Vector2Int Offset;

		RectInt() noexcept;
		RectInt(const Vector2Int& offset, const SizeInt& size) noexcept;
		RectInt(const Vector2Int& p0, const Vector2Int& p1) noexcept;

		/// @brief Gets the bottom-left corner of this rectangle
		/// @return The bottom-left corner of this rectangle
		Vector2Int GetMin() const noexcept { return Offset; }

		/// @brief Gets the top-right corner of this rectangle
		/// @return The top-right corner of this rectangle
		Vector2Int GetMax() const noexcept { return Offset + Size; }

		/// @brief Gets the center of this rectangle
		/// @return The center of this rectangle
		Vector2 GetCenter() const noexcept { return static_cast<Vector2>(Offset) + Vector2(Size.Width / 2.0, Size.Height / 2.0); }

		/// @brief Gets the position of the left edge of this rectangle
		/// @return The position of the left edge of this rectangle
		constexpr int GetLeft() const noexcept { return Offset.X; }

		/// @brief Gets the position of the right edge of this rectangle
		/// @return The position of the right edge of this rectangle
		constexpr int GetRight() const noexcept { return Offset.X + Size.Width; }

		/// @brief Gets the position of the bottom edge of this rectangle
		/// @return The position of the bottom edge of this rectangle
		constexpr int GetBottom() const noexcept { return Offset.Y; }

		/// @brief Gets the position of the top edge of this rectangle
		/// @return The position of the top edge of this rectangle
		constexpr int GetTop() const noexcept { return Offset.Y + Size.Height; }

		/// @brief Expands this rectangle to encompass the given point
		/// @param point The point for this rectangle to cover
		void Expand(const Vector2Int& point) noexcept;

		/// @brief Tests if this rectangle intersects with another rectangle
		/// @param other The other rectangle
		/// @return True if there is an intersection
		bool Intersects(const RectInt& other) noexcept;

		/// @brief Tests if this rectangle intersects with a point
		/// @param point The point
		/// @return True if there is an intersection
		bool Intersects(const Vector2Int& point) noexcept;
	};

	/// @brief A rectangle that uses decinal coordinates
	struct COCOAPI Rect
	{
		/// @brief The size of the rectangle
		Size Size;

		/// @brief The bottom-left position of the rectangle
		Vector2 Offset;

		Rect() noexcept;
		Rect(const Vector2& offset, const Coco::Size& size) noexcept;
		Rect(const Vector2& p0, const Vector2& p1) noexcept;

		/// @brief Gets the bottom-left corner of this rectangle
		/// @return The bottom-left corner of this rectangle
		Vector2 GetMin() const noexcept { return Offset; }

		/// @brief Gets the top-right corner of this rectangle
		/// @return The top-right corner of this rectangle
		Vector2 GetMax() const noexcept { return Offset + Size; }

		/// @brief Gets the center of this rectangle
		/// @return The center of this rectangle
		Vector2 GetCenter() const noexcept { return Offset + Vector2(Size.Width / 2.0, Size.Height / 2.0); }

		/// @brief Gets the position of the left edge of this rectangle
		/// @return The position of the left edge of this rectangle
		constexpr double GetLeft() const noexcept { return Offset.X; }

		/// @brief Gets the position of the right edge of this rectangle
		/// @return The position of the right edge of this rectangle
		constexpr double GetRight() const noexcept { return Offset.X + Size.Width; }

		/// @brief Gets the position of the bottom edge of this rectangle
		/// @return The position of the bottom edge of this rectangle
		constexpr double GetBottom() const noexcept { return Offset.Y; }

		/// @brief Gets the position of the top edge of this rectangle
		/// @return The position of the top edge of this rectangle
		constexpr double GetTop() const noexcept { return Offset.Y + Size.Height; }

		/// @brief Expands this rectangle to encompass the given point
		/// @param point The point for this rectangle to cover
		void Expand(const Vector2& point) noexcept;

		/// @brief Tests if this rectangle intersects with another rectangle
		/// @param other The other rectangle
		/// @return True if there is an intersection
		bool Intersects(const Rect& other) noexcept;

		/// @brief Tests if this rectangle intersects with a point
		/// @param point The point
		/// @return True if there is an intersection
		bool Intersects(const Vector2& point) noexcept;
	};
}