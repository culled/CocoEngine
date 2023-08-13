#include "Rect.h"

#include <Coco/Core/Math/Math.h>

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

	Vector2 Rect::GetNormalOfSide(const RectangleSide& side)
	{
		switch (side)
		{
		case RectangleSide::Left:
			return Vector2::Left;
		case RectangleSide::Top:
			return Vector2::Up;
		case RectangleSide::Right:
			return Vector2::Right;
		default:
			return Vector2::Down;
		}
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

	bool Rect::Intersects(const Rect& other) const noexcept
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
		//return ((r1 >= r2 && r1 <= l2) || (l1 >= r2 && l1 <= l2)) && ((b1 >= b2 && b1 <= t2) || (t1 >= b2 && t1 <= t2));
		// https://stackoverflow.com/questions/27025613/c-intersection-between-custom-rectangular-class
		return r2 >= l1 && b2 <= t1 && t2 >= b1 && l2 <= r1;
	}

	bool Rect::Intersects(const Vector2& point) const noexcept
	{
		const double r1 = GetRight() - Math::Epsilon;
		const double l1 = GetLeft() + Math::Epsilon;
		const double b1 = GetBottom() - Math::Epsilon;
		const double t1 = GetTop() + Math::Epsilon;

		// Check if the point lies within the width and height of this rectangle
		return (point.X >= r1 && point.X <= l1) && (point.Y >= b1 && point.Y <= t1);
	}

	Rect Rect::GetIntersection(const Rect& other) const noexcept
	{
		if (!Intersects(other))
			return Rect();

		double l1 = GetLeft();
		double l2 = other.GetLeft();
		double r1 = GetRight();
		double r2 = other.GetRight();
		double b1 = GetBottom();
		double b2 = other.GetBottom();
		double t1 = GetTop();
		double t2 = other.GetTop();

		Vector2 bottomLeft(Math::Max(l1, l2), Math::Max(b1, b2));
		Vector2 topRight(Math::Min(r1, r2), Math::Min(t1, t2));

		return Rect(bottomLeft, topRight);
	}

	Vector2 Rect::GetClosestPoint(const Vector2& point, RectangleSide* side) const
	{
		const double clampedX = Math::Clamp(point.X, GetLeft(), GetRight());
		const double clampedY = Math::Clamp(point.Y, GetBottom(), GetTop());

		Vector2 left(Offset.X, clampedY);
		Vector2 right(Offset.X + Size.Width, clampedY);
		Vector2 top(clampedX, Offset.Y + Size.Height);
		Vector2 bottom(clampedX, Offset.Y);

		double minDist = Vector2::DistanceBetween(point, left);
		RectangleSide s = RectangleSide::Left;

		double d = Vector2::DistanceBetween(point, top);
		if (d < minDist)
		{
			s = RectangleSide::Top;
			minDist = d;
		}

		d = Vector2::DistanceBetween(point, right);
		if (d < minDist)
		{
			s = RectangleSide::Right;
			minDist = d;
		}

		d = Vector2::DistanceBetween(point, bottom);
		if (d < minDist)
		{
			s = RectangleSide::Bottom;
			minDist = d;
		}

		if (side != nullptr)
			*side = s;

		switch (s)
		{
		case RectangleSide::Left:
			return left;
		case RectangleSide::Top:
			return top;
		case RectangleSide::Right:
			return right;
		default:
			return bottom;
		}
	}

	Vector2 Rect::GetClosestPoint(const Rect& other, RectangleSide* side) const
	{
		// TODO: I feel like this function could do with some improvements...
		if (!Intersects(other))
		{
			double x1 = other.GetRight() - GetLeft();
			double x2 = GetRight() - other.GetLeft();
			double cx;

			if (x1 < x2)
			{
				cx = GetLeft() + x1 * 0.5;
			}
			else
			{
				cx = other.GetLeft() + x2 * 0.5;
			}

			double y1 = other.GetTop() - GetBottom();
			double y2 = GetTop() - other.GetBottom();
			double cy;

			if (y1 < y2)
			{
				cy = GetBottom() + y1 * 0.5;
			}
			else
			{
				cy = other.GetBottom() + y2 * 0.5;
			}

			return GetClosestPoint(Vector2(cx, cy), side);
		}
		
		Rect i = GetIntersection(other);

		bool isHorizontal = i.Size.Width < i.Size.Height;
		Vector2 point;
		RectangleSide s;

		if (isHorizontal)
		{
			point.Y = i.GetCenter().Y;

			if (Math::Approximately(i.GetLeft(), GetLeft()))
			{
				point.X = i.GetLeft();
				s = RectangleSide::Left;
			}
			else
			{
				point.X = i.GetRight();
				s = RectangleSide::Right;
			}
		}
		else
		{
			point.X = i.GetCenter().X;

			if (Math::Approximately(i.GetBottom(), GetBottom()))
			{
				point.Y = i.GetBottom();
				s = RectangleSide::Bottom;
			}
			else
			{
				point.Y = i.GetTop();
				s = RectangleSide::Top;
			}
		}

		if (side != nullptr)
			*side = s;

		return point;
	}
}
