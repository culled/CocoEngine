#include "Corepch.h"
#include "BoundingBox.h"
#include "Matrix.h"

#include "../Math/Math.h"

namespace Coco
{
	const BoundingBox BoundingBox::Zero = BoundingBox();

	BoundingBox::BoundingBox() :
		BoundingBox(Vector3::Zero, Vector3::Zero)
	{}

	BoundingBox::BoundingBox(const Vector3& p0, const Vector3& p1) :
		Minimum(Math::Min(p0.X, p1.X), Math::Min(p0.Y, p1.Y), Math::Min(p0.Z, p1.Z)),
		Maximum(Math::Max(p0.X, p1.X), Math::Max(p0.Y, p1.Y), Math::Max(p0.Z, p1.Z))
	{}

	BoundingBox BoundingBox::FromCenterAndSize(const Vector3& center, const Vector3& size)
	{
		Vector3 halfSize = size * 0.5;

		return BoundingBox(center - halfSize, center + halfSize);
	}

	BoundingBox BoundingBox::FromPoints(std::span<const Vector3> points)
	{
		BoundingBox box;

		bool first = true;

		for (const Vector3& p : points)
		{
			if (first)
			{
				box.Minimum = p;
				box.Maximum = p;
				first = false;
			}
			else
			{
				box.Expand(p);
			}
		}

		return box;
	}

	Vector3 BoundingBox::GetCenter() const
	{
		return (Maximum + Minimum) * 0.5;
	}

	Vector3 BoundingBox::GetSize() const
	{
		return Maximum - Minimum;
	}

	double BoundingBox::GetVolume() const
	{
		Vector3 s = GetSize();
		return s.X * s.Y * s.Z;
	}

	void BoundingBox::Translate(const Vector3& translation)
	{
		Minimum += translation;
		Maximum += translation;
	}

	void BoundingBox::Expand(const Vector3& point)
	{
		Minimum.X = Math::Min(Minimum.X, point.X);
		Minimum.Y = Math::Min(Minimum.Y, point.Y);
		Minimum.Z = Math::Min(Minimum.Z, point.Z);

		Maximum.X = Math::Max(Maximum.X, point.X);
		Maximum.Y = Math::Max(Maximum.Y, point.Y);
		Maximum.Z = Math::Max(Maximum.Z, point.Z);
	}

	void BoundingBox::Expand(const BoundingBox& box)
	{
		Minimum.X = Math::Min(Minimum.X, box.Minimum.X);
		Minimum.Y = Math::Min(Minimum.Y, box.Minimum.Y);
		Minimum.Z = Math::Min(Minimum.Z, box.Minimum.Z);

		Maximum.X = Math::Max(Maximum.X, box.Maximum.X);
		Maximum.Y = Math::Max(Maximum.Y, box.Maximum.Y);
		Maximum.Z = Math::Max(Maximum.Z, box.Maximum.Z);
	}

	void BoundingBox::Inflate(const Vector3& amount)
	{
		Vector3 halfAmount = amount * 0.5;

		Minimum -= halfAmount;
		Maximum += halfAmount;

		if (Minimum.X > Maximum.X)
		{
			double x = Maximum.X;
			Maximum.X = Minimum.X;
			Minimum.X = x;
		}

		if (Minimum.Y > Maximum.Y)
		{
			double y = Maximum.Y;
			Maximum.Y = Minimum.Y;
			Minimum.Y = y;
		}

		if (Minimum.Z > Maximum.Z)
		{
			double z = Maximum.Z;
			Maximum.Z = Minimum.Z;
			Minimum.Z = z;
		}
	}

	bool BoundingBox::Intersects(const Vector3& point)
	{
		return point.X >= Minimum.X - Math::LaxEpsilon &&
			point.X <= Maximum.X + Math::LaxEpsilon &&
			point.Y >= Minimum.Y - Math::LaxEpsilon &&
			point.Y <= Maximum.Y + Math::LaxEpsilon &&
			point.Z >= Minimum.Z - Math::LaxEpsilon &&
			point.Z <= Maximum.Z + Math::LaxEpsilon;
	}

	bool BoundingBox::Intersects(const BoundingBox& other)
	{
		return Minimum.X <= other.Maximum.X + Math::LaxEpsilon &&
			Maximum.X >= other.Minimum.X - Math::LaxEpsilon &&
			Minimum.Y <= other.Maximum.Y + Math::LaxEpsilon &&
			Maximum.Y >= other.Minimum.Y - Math::LaxEpsilon &&
			Minimum.Z <= other.Maximum.Z + Math::LaxEpsilon &&
			Maximum.Z >= other.Minimum.Z - Math::LaxEpsilon;
	}

	BoundingBox BoundingBox::Transformed(const Matrix4x4& trsMatrix) const
	{
		std::array<Vector4, 8> points = {
			trsMatrix * Vector4(Minimum, 1.0),
			trsMatrix * Vector4(Maximum.X, Minimum.Y, Minimum.Z, 1.0),
			trsMatrix * Vector4(Minimum.X, Maximum.Y, Minimum.Z, 1.0),
			trsMatrix * Vector4(Minimum.X, Minimum.Y, Maximum.Z, 1.0),
			trsMatrix * Vector4(Maximum.X, Maximum.Y, Minimum.Z, 1.0),
			trsMatrix * Vector4(Maximum.X, Minimum.Y, Maximum.Z, 1.0),
			trsMatrix * Vector4(Minimum.X, Maximum.Y, Maximum.Z, 1.0),
			trsMatrix * Vector4(Maximum, 1.0)
		};

		Vector3 min;
		Vector3 max;
		bool first = true;

		for (const Vector4& p : points)
		{
			if (first)
			{
				min = p.XYZ();
				max = min;
				first = false;
			}
			else
			{
				min.X = Math::Min(min.X, p.X);
				min.Y = Math::Min(min.Y, p.Y);
				min.Z = Math::Min(min.Z, p.Z);

				max.X = Math::Max(max.X, p.X);
				max.Y = Math::Max(max.Y, p.Y);
				max.Z = Math::Max(max.Z, p.Z);
			}
		}

		return BoundingBox(min, max);
	}

	std::array<Vector3, 8> BoundingBox::GetCorners() const
	{
		return {
			Vector3(Minimum),
			Vector3(Maximum.X, Minimum.Y, Minimum.Z),
			Vector3(Minimum.X, Maximum.Y, Minimum.Z),
			Vector3(Minimum.X, Minimum.Y, Maximum.Z),
			Vector3(Maximum.X, Maximum.Y, Minimum.Z),
			Vector3(Maximum.X, Minimum.Y, Maximum.Z),
			Vector3(Minimum.X, Maximum.Y, Maximum.Z),
			Vector3(Maximum)
		};
	}
}