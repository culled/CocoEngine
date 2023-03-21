#include "Rect.h"

namespace Coco
{
	RectInt::RectInt() noexcept :
		Size(0, 0), Offset(0, 0)
	{}

	RectInt::RectInt(const Vector2Int & offset, const SizeInt & size) noexcept :
		Offset(offset), Size(size)
	{}

	RectInt::RectInt(const Vector2Int& p0, const Vector2Int & p1) noexcept
	{
		const int left =	Math::Min(p0.X, p1.X);
		const int right =	Math::Max(p0.X, p1.X);
		const int bottom =	Math::Min(p0.Y, p1.Y);
		const int top =		Math::Max(p0.Y, p1.Y);

		Offset = Vector2Int(left, bottom);
		Size = SizeInt(right - left, top - bottom);
	}

	void RectInt::Expand(const Vector2Int& point) noexcept
	{
		const int leftMove =	Math::Max(Offset.X - point.X, 0);
		const int bottomMove =	Math::Max(Offset.Y - point.Y, 0);

		if (leftMove > 0)
		{
			Offset.X -= leftMove;
			Size.Width += leftMove;
		}
		else
		{
			Size.Width += Math::Max(point.X - GetRight(), 0);
		}

		if (bottomMove > 0)
		{
			Offset.Y -= bottomMove;
			Size.Height += bottomMove;
		}
		else
		{
			Size.Height += Math::Max(point.Y - GetTop(), 0);
		}
	}

	bool RectInt::Intersects(const RectInt& other) noexcept
	{
		const int r1 = GetRight();
		const int r2 = other.GetRight();
		const int l1 = GetLeft();
		const int l2 = other.GetLeft();
		const int b1 = GetBottom();
		const int b2 = other.GetBottom();
		const int t1 = GetTop();
		const int t2 = other.GetTop();

		// Check if each corner of the first rectangle lies within the width and height of the second
		return ((r1 >= r2 && r1 <= l2) || (l1 >= r2 && l1 <= l2)) && ((b1 >= b2 && b1 <= t2) || (t1 >= b2 && t1 <= t2));
	}

	bool RectInt::Intersects(const Vector2Int& point) noexcept
	{
		const int r1 = GetRight();
		const int l1 = GetLeft();
		const int b1 = GetBottom();
		const int t1 = GetTop();

		// Check if the point lies within the width and height of this rectangle
		return (point.X >= r1 && point.X <= l1) && (point.Y >= b1 && point.Y <= t1);
	}

	Rect::Rect() noexcept : Offset(0.0, 0.0), Size(0.0, 0.0)
	{}

	Rect::Rect(const Vector2 & offset, const Coco::Size & size) noexcept : Offset(offset), Size(size)
	{}

	Rect::Rect(const Vector2 & p0, const Vector2 & p1) noexcept
	{
		const double left = Math::Min(p0.X, p1.X);
		const double right = Math::Max(p0.X, p1.X);
		const double bottom = Math::Min(p0.Y, p1.Y);
		const double top = Math::Max(p0.Y, p1.Y);

		Offset = Vector2(left, bottom);
		Size = Coco::Size(right - left, top - bottom);
	}

	void Rect::Expand(const Vector2& point) noexcept
	{
		const double leftMove = Math::Max(Offset.X - point.X, 0.0);
		const double bottomMove = Math::Max(Offset.Y - point.Y, 0.0);

		if (leftMove > 0)
		{
			Offset.X -= leftMove;
			Size.Width += leftMove;
		}
		else
		{
			Size.Width += Math::Max(point.X - GetRight(), 0.0);
		}

		if (bottomMove > 0)
		{
			Offset.Y -= bottomMove;
			Size.Height += bottomMove;
		}
		else
		{
			Size.Height += Math::Max(point.Y - GetTop(), 0.0);
		}
	}

	bool Rect::Intersects(const Rect& other) noexcept
	{
		const double r1 = GetRight() - Math::Epsilon;
		const double l1 = GetLeft() + Math::Epsilon;
		const double b1 = GetBottom() - Math::Epsilon;
		const double t1 = GetTop() + Math::Epsilon;
		const double r2 = other.GetRight();
		const double l2 = other.GetLeft();
		const double b2 = other.GetBottom();
		const double t2 = other.GetTop();

		// Check if each corner of the first rectangle lies within the width and height of the second
		return ((r1 >= r2 && r1 <= l2) || (l1 >= r2 && l1 <= l2)) && ((b1 >= b2 && b1 <= t2) || (t1 >= b2 && t1 <= t2));
	}

	bool Rect::Intersects(const Vector2& point) noexcept
	{
		const double r1 = GetRight() - Math::Epsilon;
		const double l1 = GetLeft() + Math::Epsilon;
		const double b1 = GetBottom() - Math::Epsilon;
		const double t1 = GetTop() + Math::Epsilon;

		// Check if the point lies within the width and height of this rectangle
		return (point.X >= r1 && point.X <= l1) && (point.Y >= b1 && point.Y <= t1);
	}
}
