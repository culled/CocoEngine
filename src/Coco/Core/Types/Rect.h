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
		/// @brief The top-right position of the rectangle
		Vector2 Maximum;

		/// @brief The bottom-left position of the rectangle
		Vector2 Minimum;

		Rect();
		Rect(const Vector2& p0, const Vector2& p1);

		/// @brief Creates a rectangle centered on a point
		/// @param center The center of the rectangle
		/// @param size The size of the rectangle
		/// @return The rectangle
		static Rect FromCenterAndSize(const Vector2& center, const Size& size);

		/// @brief Creates a rectangles that encompasses all the given points
		/// @param points The points
		/// @return The rectangle
		static Rect FromPoints(std::span<const Vector2> points);

		/// @brief Gets the bottom-left corner of this rectangle
		/// @return The bottom-left corner of this rectangle
		const Vector2& GetMin() const { return Minimum; }

		/// @brief Gets the top-right corner of this rectangle
		/// @return The top-right corner of this rectangle
		const Vector2& GetMax() const { return Maximum; }

		/// @brief Gets the center of this rectangle
		/// @return The center of this rectangle
		Vector2 GetCenter() const;

		/// @brief Gets the size of this rectangle
		/// @return The size of this rectangle
		Size GetSize() const;

		/// @brief Calculates the area of this rectangle
		/// @return The area
		double GetArea() const;

		/// @brief Gets the position of the left edge of this rectangle
		/// @return The position of the left edge of this rectangle
		constexpr double GetLeft() const { return Minimum.X; }

		/// @brief Gets the position of the right edge of this rectangle
		/// @return The position of the right edge of this rectangle
		constexpr double GetRight() const { return Maximum.X; }

		/// @brief Gets the position of the bottom edge of this rectangle
		/// @return The position of the bottom edge of this rectangle
		constexpr double GetBottom() const { return Minimum.Y; }

		/// @brief Gets the position of the top edge of this rectangle
		/// @return The position of the top edge of this rectangle
		constexpr double GetTop() const { return Maximum.Y; }

		/// @brief Moves this rectangle by an amount
		/// @param translation The amount to move
		void Translate(const Vector2& translation);

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

		/// @brief Gets the aspect ratio of this rectangle (width / heigth)
		/// @return The aspect ratio
		double GetAspectRatio() const;
	};

	/// @brief An axis-aligned rectangle backed by int values
	struct RectInt
	{
		/// @brief The top-right position of the rectangle
		Vector2Int Maximum;

		/// @brief The bottom-left position of the rectangle
		Vector2Int Minimum;

		RectInt();
		RectInt(const Vector2Int& p0, const Vector2Int& p1);

		operator Rect() { return Rect(Minimum, Maximum); }

		/// @brief Creates a rectangles that encompasses all the given points
		/// @param points The points
		/// @return The rectangle
		static RectInt FromPoints(std::span<const Vector2Int> points);

		/// @brief Gets the bottom-left corner of this rectangle
		/// @return The bottom-left corner of this rectangle
		Vector2Int GetMin() const { return Minimum; }

		/// @brief Gets the top-right corner of this rectangle
		/// @return The top-right corner of this rectangle
		Vector2Int GetMax() const { return Maximum; }

		/// @brief Gets the center of this rectangle
		/// @return The center of this rectangle
		Vector2 GetCenter() const;

		/// @brief Gets the size of this rectangle
		/// @return The size of this rectangle
		SizeInt GetSize() const;

		/// @brief Calculates the area of this rectangle
		/// @return The area
		int GetArea() const;

		/// @brief Gets the position of the left edge of this rectangle
		/// @return The position of the left edge of this rectangle
		constexpr int GetLeft() const { return Minimum.X; }

		/// @brief Gets the position of the right edge of this rectangle
		/// @return The position of the right edge of this rectangle
		constexpr int GetRight() const { return Maximum.X; }

		/// @brief Gets the position of the bottom edge of this rectangle
		/// @return The position of the bottom edge of this rectangle
		constexpr int GetBottom() const { return Minimum.Y; }

		/// @brief Gets the position of the top edge of this rectangle
		/// @return The position of the top edge of this rectangle
		constexpr int GetTop() const { return Maximum.Y; }

		/// @brief Moves this rectangle by an amount
		/// @param translation The amount to move
		void Translate(const Vector2Int& translation);

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

		/// @brief Gets the aspect ratio of this rectangle (width / heigth)
		/// @return The aspect ratio
		double GetAspectRatio() const;
	};
}