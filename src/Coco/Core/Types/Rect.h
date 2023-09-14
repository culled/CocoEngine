#pragma once

#include "Size.h"
#include "Vector.h"

namespace Coco
{
	/// @brief Sides of a rectangle
	enum class RectangleSide
	{
		Left,
		Top,
		Right,
		Bottom
	};

	/// @brief An axis-aligned rectangle backed by double values
	struct Rect
	{
		/// @brief The size of the rectangle
		Size Size;

		/// @brief The bottom-left position of the rectangle
		Vector2 Offset;

		Rect();
		Rect(const Vector2& offset, const Coco::Size& size);
		Rect(const Vector2& p0, const Vector2& p1);

		/// @brief Gets the bottom-left corner of this rectangle
		/// @return The bottom-left corner of this rectangle
		Vector2 GetMin() const { return Offset; }

		/// @brief Gets the top-right corner of this rectangle
		/// @return The top-right corner of this rectangle
		Vector2 GetMax() const { return Offset + Size; }

		/// @brief Gets the center of this rectangle
		/// @return The center of this rectangle
		Vector2 GetCenter() const { return Offset + Vector2(Size.Width / 2.0, Size.Height / 2.0); }

		/// @brief Gets the position of the left edge of this rectangle
		/// @return The position of the left edge of this rectangle
		constexpr double GetLeft() const { return Offset.X; }

		/// @brief Gets the position of the right edge of this rectangle
		/// @return The position of the right edge of this rectangle
		constexpr double GetRight() const { return Offset.X + Size.Width; }

		/// @brief Gets the position of the bottom edge of this rectangle
		/// @return The position of the bottom edge of this rectangle
		constexpr double GetBottom() const { return Offset.Y; }

		/// @brief Gets the position of the top edge of this rectangle
		/// @return The position of the top edge of this rectangle
		constexpr double GetTop() const { return Offset.Y + Size.Height; }

		/// @brief Expands this rectangle to encompass the given point
		/// @param point The point for this rectangle to cover
		void Expand(const Vector2& point);

		/// @brief Tests if this rectangle intersects with another rectangle
		/// @param other The other rectangle
		/// @return True if there is an intersection
		bool Intersects(const Rect& other) const;

		/// @brief Tests if this rectangle intersects with a point
		/// @param point The point
		/// @return True if there is an intersection
		bool Intersects(const Vector2& point) const;

		/// @brief Gets the intersecting area between this rectangle and another
		/// @param other The other rectangle
		/// @return The intersecting area. Or if the rectangles do not intersect, will be the smallest rectangle between the two
		Rect GetIntersection(const Rect& other) const;

		/// @brief Gets the closest point on this rectangle relative to a point
		/// @param point The point
		/// @param outSide If given, will be set to the side that the point lies on
		/// @return The closest point
		Vector2 GetClosestPoint(const Vector2& point, RectangleSide* outSide = nullptr) const;

		/// @brief Gets the closest point on this rectangle relative to another rectangle
		/// @param other The other rectangle
		/// @param outSide If given, will be set to the side that the point lies on
		/// @return The closest point
		Vector2 GetClosestPoint(const Rect& other, RectangleSide* outSide = nullptr) const;
	};

	/// @brief An axis-aligned rectangle backed by int values
	struct RectInt
	{
		/// @brief The size of the rectangle
		SizeInt Size;

		/// @brief The bottom-left position of the rectangle
		Vector2Int Offset;

		RectInt();
		RectInt(const Vector2Int& offset, const SizeInt& size);
		RectInt(const Vector2Int& p0, const Vector2Int& p1);

		operator Rect() { return Rect(Offset, Size); }

		/// @brief Gets the bottom-left corner of this rectangle
		/// @return The bottom-left corner of this rectangle
		Vector2Int GetMin() const { return Offset; }

		/// @brief Gets the top-right corner of this rectangle
		/// @return The top-right corner of this rectangle
		Vector2Int GetMax() const { return Offset + Size; }

		/// @brief Gets the center of this rectangle
		/// @return The center of this rectangle
		Vector2 GetCenter() const { return static_cast<Vector2>(Offset) + Vector2(Size.Width / 2.0, Size.Height / 2.0); }

		/// @brief Gets the position of the left edge of this rectangle
		/// @return The position of the left edge of this rectangle
		constexpr int GetLeft() const { return Offset.X; }

		/// @brief Gets the position of the right edge of this rectangle
		/// @return The position of the right edge of this rectangle
		constexpr int GetRight() const { return Offset.X + Size.Width; }

		/// @brief Gets the position of the bottom edge of this rectangle
		/// @return The position of the bottom edge of this rectangle
		constexpr int GetBottom() const { return Offset.Y; }

		/// @brief Gets the position of the top edge of this rectangle
		/// @return The position of the top edge of this rectangle
		constexpr int GetTop() const { return Offset.Y + Size.Height; }

		/// @brief Expands this rectangle to encompass the given point
		/// @param point The point for this rectangle to cover
		void Expand(const Vector2Int& point);

		/// @brief Tests if this rectangle intersects with another rectangle
		/// @param other The other rectangle
		/// @return True if there is an intersection
		bool Intersects(const RectInt& other) const;

		/// @brief Tests if this rectangle intersects with a point
		/// @param point The point
		/// @return True if there is an intersection
		bool Intersects(const Vector2& point) const;

		/// @brief Gets the intersecting area between this rectangle and another
		/// @param other The other rectangle
		/// @return The intersecting area. Or if the rectangles do not intersect, will be the smallest rectangle between the two
		RectInt GetIntersection(const RectInt& other) const;

		/// @brief Gets the closest point on this rectangle relative to a point
		/// @param point The point
		/// @param outSide If given, will be set to the side that the point lies on
		/// @return The closest point
		Vector2 GetClosestPoint(const Vector2& point, RectangleSide* outSide = nullptr) const;

		/// @brief Gets the closest point on this rectangle relative to another rectangle
		/// @param other The other rectangle
		/// @param outSide If given, will be set to the side that the point lies on
		/// @return The closest point
		Vector2 GetClosestPoint(const RectInt& other, RectangleSide* outSide = nullptr) const;
	};
}