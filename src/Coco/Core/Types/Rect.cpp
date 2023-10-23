#include "Corepch.h"
#include "Rect.h"

#include "../Math/Math.h"

namespace Coco
{
	Rect::Rect() : 
		Rect(Vector2::Zero, Vector2::Zero)
	{}

	Rect::Rect(const Vector2& p0, const Vector2& p1) :
		Minimum(Math::Min(p0.X, p1.X), Math::Min(p0.Y, p1.Y)),
		Maximum(Math::Max(p0.X, p1.X), Math::Max(p0.Y, p1.Y))
	{}

	Rect Rect::FromCenterAndSize(const Vector2& center, const Size& size)
	{
		Size halfSize = size * 0.5;
		return Rect(center - halfSize, center + halfSize);
	}

	Rect Rect::FromPoints(std::span<const Vector2> points)
	{
		Rect rect;

		bool first = true;

		for (const Vector2& p : points)
		{
			if (first)
			{
				rect = Rect(p, p);
				first = false;
			}
			else
			{
				rect.Expand(p);
			}
		}

		return rect;
	}

	Vector2 Rect::GetCenter() const
	{
		return (Minimum + Maximum) * 0.5;
	}

	Size Rect::GetSize() const
	{
		return Size(Maximum.X - Minimum.X, Maximum.Y - Minimum.Y);
	}

	double Rect::GetArea() const
	{
		Size s = GetSize();

		return s.Width * s.Height;
	}

	void Rect::Translate(const Vector2& translation)
	{
		Minimum += translation;
		Maximum += translation;
	}

	void Rect::Expand(const Vector2& point)
	{
		Minimum.X = Math::Min(Minimum.X, point.X);
		Minimum.Y = Math::Min(Minimum.Y, point.Y);

		Maximum.X = Math::Max(Maximum.X, point.X);
		Maximum.Y = Math::Max(Maximum.Y, point.Y);
	}

	bool Rect::Intersects(const Rect& other) const
	{
		// https://stackoverflow.com/questions/27025613/c-intersection-between-custom-rectangular-class
		return Minimum.X <= other.Maximum.X + Math::LaxEpsilon &&
			Maximum.X >= other.Minimum.X - Math::LaxEpsilon &&
			Minimum.Y <= other.Maximum.Y + Math::LaxEpsilon &&
			Maximum.Y >= other.Minimum.Y - Math::LaxEpsilon;
	}

	bool Rect::Intersects(const Vector2& point) const
	{
		// Check if the point lies within the width and height of this rectangle
		return point.X >= Minimum.X - Math::LaxEpsilon &&
			point.X <= Maximum.X + Math::LaxEpsilon && 
			point.Y >= Minimum.Y - Math::LaxEpsilon &&
			point.Y <= Maximum.Y + Math::LaxEpsilon;
	}

	Rect Rect::GetIntersection(const Rect& other) const
	{
		Vector2 bottomLeft(Math::Max(Minimum.X, other.Minimum.X), Math::Max(Minimum.Y, other.Minimum.Y));
		Vector2 topRight(Math::Min(Maximum.X, other.Maximum.X), Math::Min(Maximum.Y, other.Maximum.Y));

		return Rect(bottomLeft, topRight);
	}

	Vector2 Rect::GetClosestPoint(const Vector2& point, RectangleSide* side) const
	{
		const double clampedX = Math::Clamp(point.X, Minimum.X, Maximum.X);
		const double clampedY = Math::Clamp(point.Y, Minimum.Y, Maximum.Y);

		Vector2 left(Minimum.X, clampedY);
		Vector2 right(Maximum.X, clampedY);
		Vector2 bottom(clampedX, Minimum.Y);
		Vector2 top(clampedX, Maximum.Y);

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
		Rect i = GetIntersection(other);
		return GetClosestPoint(i.GetCenter(), side);
	}

	double Rect::GetAspectRatio() const
	{
		Size size = GetSize();
		return size.Width / size.Height;
	}

	RectInt::RectInt() : RectInt(Vector2Int::Zero, SizeInt::Zero)
	{}

	RectInt::RectInt(const Vector2Int& p0, const Vector2Int& p1) :
		Minimum(Math::Min(p0.X, p1.X), Math::Min(p0.Y, p1.Y)),
		Maximum(Math::Max(p0.X, p1.X), Math::Max(p0.Y, p1.Y))
	{}

	RectInt RectInt::FromPoints(std::span<const Vector2Int> points)
	{
		RectInt rect;

		bool first = true;

		for (const Vector2Int& p : points)
		{
			if (first)
			{
				rect = RectInt(p, p);
				first = false;
			}
			else
			{
				rect.Expand(p);
			}
		}

		return rect;
	}

	Vector2 RectInt::GetCenter() const
	{
		return static_cast<Vector2>(Maximum + Minimum) * 0.5;
	}

	SizeInt RectInt::GetSize() const
	{
		return SizeInt(Maximum.X - Minimum.X, Maximum.Y - Minimum.Y);
	}

	int RectInt::GetArea() const
	{
		SizeInt size = GetSize();
		return size.Width * size.Height;
	}

	void RectInt::Translate(const Vector2Int& translation)
	{
		Minimum += translation;
		Maximum += translation;
	}

	void RectInt::Expand(const Vector2Int& point)
	{
		Minimum.X = Math::Min(Minimum.X, point.X);
		Minimum.Y = Math::Min(Minimum.Y, point.Y);

		Maximum.X = Math::Max(Maximum.X, point.X);
		Maximum.Y = Math::Max(Maximum.Y, point.Y);
	}

	bool RectInt::Intersects(const RectInt& other) const
	{
		// https://stackoverflow.com/questions/27025613/c-intersection-between-custom-rectangular-class
		return Minimum.X <= other.Maximum.X &&
			Maximum.X >= other.Minimum.X &&
			Minimum.Y <= other.Maximum.Y &&
			Maximum.Y >= other.Minimum.Y;
	}

	bool RectInt::Intersects(const Vector2& point) const
	{
		// Check if the point lies within the width and height of this rectangle
		return point.X + Math::Epsilon >= Minimum.X &&
			point.X - Math::Epsilon <= Maximum.X &&
			point.Y + Math::Epsilon >= Minimum.Y &&
			point.Y - Math::Epsilon <= Maximum.Y;
	}

	RectInt RectInt::GetIntersection(const RectInt& other) const
	{
		Vector2Int bottomLeft(Math::Max(Minimum.X, other.Minimum.X), Math::Max(Minimum.Y, other.Minimum.Y));
		Vector2Int topRight(Math::Min(Maximum.X, other.Maximum.X), Math::Min(Maximum.Y, other.Maximum.Y));

		return RectInt(bottomLeft, topRight);
	}

	Vector2 RectInt::GetClosestPoint(const Vector2& point, RectangleSide* side) const
	{
		const double clampedX = Math::Clamp(point.X, static_cast<double>(Minimum.X), static_cast<double>(Maximum.X));
		const double clampedY = Math::Clamp(point.Y, static_cast<double>(Minimum.Y), static_cast<double>(Maximum.Y));

		Vector2 left(Minimum.X, clampedY);
		Vector2 right(Maximum.X, clampedY);
		Vector2 bottom(clampedX, Minimum.Y);
		Vector2 top(clampedX, Maximum.Y);

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

	Vector2 RectInt::GetClosestPoint(const RectInt& other, RectangleSide* side) const
	{
		RectInt i = GetIntersection(other);
		return GetClosestPoint(i.GetCenter(), side);
	}

	double RectInt::GetAspectRatio() const
	{
		SizeInt size = GetSize();
		return static_cast<double>(size.Width) / size.Height;
	}
}