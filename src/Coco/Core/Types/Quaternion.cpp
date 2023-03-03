#include "Quaternion.h"
#include <Coco/Core/Math/Math.h>

namespace Coco
{
	const Quaternion Quaternion::Identity = Quaternion(0.0, 0.0, 0.0, 1.0);

	Quaternion::Quaternion() : Quaternion(0.0, 0.0, 0.0, 1.0)
	{}

	Quaternion::Quaternion(double x, double y, double z, double w) :
		X(x), Y(y), Z(z), W(w)
	{}

	double Quaternion::Dot(const Quaternion& a, const Quaternion& b)
	{
		return a.Dot(b);
	}

	double Quaternion::Normal(bool safe) const
	{
		if (safe &&
			Math::Approximately(X, 0.0) &&
			Math::Approximately(Y, 0.0) &&
			Math::Approximately(Z, 0.0) &&
			Math::Approximately(W, 0.0))
			return 0.0;

		return Math::Sqrt(X * X + Y * Y + Z * Z + W * W);
	}

	void Quaternion::Normalize(bool safe)
	{
		const double l = Normal(safe);
		X /= l;
		Y /= l;
		Z /= l;
		W /= l;
	}

	Quaternion Quaternion::Conjugate() const
	{
		return Quaternion(-X, -Y, -Z, W);
	}
	Quaternion Quaternion::Inverted() const
	{
		Quaternion inv = Conjugate();
		inv.Normalize();
		return inv;
	}

	double Quaternion::Dot(const Quaternion& other) const
	{
		double p = 0.0;
		p += X * other.X;
		p += Y * other.Y;
		p += Z * other.Z;
		p += W * other.W;
		return p;
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		Quaternion result;

		result.X = X * other.W +
			Y * other.Z -
			Z * other.Y +
			W * other.X;

		result.Y = -X * other.Z +
			Y * other.W +
			Z * other.X +
			W * other.Y;

		result.Z = X * other.Y -
			Y * other.X +
			Z * other.W +
			W * other.Z;

		result.W = -X * other.X -
			Y * other.Y -
			Z * other.Z +
			W * other.W;

		return result;
	}
}