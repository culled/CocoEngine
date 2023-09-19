#include "Corepch.h"
#include "Quaternion.h"

namespace Coco
{
	const Quaternion Quaternion::Identity = Quaternion(0, 0, 0, 1);

	Quaternion::Quaternion() :
		Quaternion(0, 0, 0, 1)
	{}

	Quaternion::Quaternion(double x, double y, double z, double w) :
		X(x),
		Y(y),
		Z(z),
		W(w)
	{}

	Quaternion::Quaternion(const Vector3& axis, double angleRadians)
	{
		const double halfAngle = angleRadians * 0.5;
		const double s = Math::Sin(halfAngle);
		const double c = Math::Cos(halfAngle);
		const Vector3 normalizedAxis = axis.Normalized();

		X = s * normalizedAxis.X;
		Y = s * normalizedAxis.Y;
		Z = s * normalizedAxis.Z;
		W = c;
	}

	Quaternion::Quaternion(const Vector3& eulerAngles)
	{
		const Vector3 halfAngles = eulerAngles * 0.5;

		// Pitch (X), Roll (Z), Yaw (Y) - YXZ order
		const double cx = Math::Cos(halfAngles.X);
		const double sx = Math::Sin(halfAngles.X);
		const double cy = Math::Cos(halfAngles.Y);
		const double sy = Math::Sin(halfAngles.Y);
		const double cz = Math::Cos(halfAngles.Z);
		const double sz = Math::Sin(halfAngles.Z);

		W = (cy * cx * cz) - (-(sy * sx) * sz);
		X = (cy * sx * cz) + (sy * cx * sz);
		Y = (sy * cx * cz) + (-(cy * sx) * sz);
		Z = (cy * cx * sz) + (-(sy * sx) * cz);
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

	Vector3 Quaternion::operator*(const Vector3& direction) const
	{
		// https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		const Vector3 u(X, Y, Z);

		return u * 2.0 * u.Dot(direction) +
			direction * (W * W - u.GetLengthSquared()) +
			u.Cross(direction) * 2.0 * W;
	}
	
	Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, double alpha)
	{
		Quaternion out;

		const Quaternion a = from.Normalized();
		Quaternion b = to.Normalized();

		double dot = a.Dot(b);

		// If the dot product is negative, slerp won't take the shortest path
		// Fix by reversing all components of one rotation
		if (dot < 0.0)
		{
			b.X = -b.X;
			b.Y = -b.Y;
			b.Z = -b.Z;
			b.W = -b.W;
			dot = -dot;
		}

		// If the inputs are really close, linearly interpolate and normalize the result
		constexpr double dotThreshold = 0.9995;
		if (dot > dotThreshold)
		{
			out.X = a.X + (b.X - a.X) * alpha;
			out.Y = a.Y + (b.Y - a.Y) * alpha;
			out.Z = a.Z + (b.Z - a.Z) * alpha;
			out.W = a.W + (b.W - a.W) * alpha;
		}
		else
		{
			const double theta0 = Math::Acos(dot);
			const double theta = theta0 * alpha;
			const double sinTheta = Math::Sin(theta);
			const double sinTheta0 = Math::Sin(theta0);

			const double s0 = Math::Cos(theta) - dot * sinTheta / sinTheta0;
			const double s1 = sinTheta / sinTheta0;

			out.X = a.X * s0 + b.X * s1;
			out.Y = a.Y * s0 + b.Y * s1;
			out.Z = a.Z * s0 + b.Z * s1;
			out.W = a.W * s0 + b.W * s1;
		}

		return out.Normalized();
	}

	void Quaternion::Normalize(bool safe)
	{
		const double l = GetNormal();

		if (safe && Math::Equal(l, 0.0))
			return;

		X /= l;
		Y /= l;
		Z /= l;
		W /= l;
	}

	Quaternion Quaternion::Normalized(bool safe) const
	{
		Quaternion copy(*this);
		copy.Normalize(safe);
		return copy;
	}

	Vector3 Quaternion::ToEulerAngles() const
	{
		Vector3 eulerAngles;

		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		// Roll (Z-axis rotation)
		double sinr_cosp = 2 * (W * Z + X * Y);
		double cosr_cosp = 1 - 2 * (Z * Z + X * X);
		eulerAngles.Z = std::atan2(sinr_cosp, cosr_cosp);

		// Pitch (X-axis rotation)
		double sinp = std::sqrt(1 + 2 * (W * X - Z * Y));
		double cosp = std::sqrt(1 - 2 * (W * X - Z * Y));
		eulerAngles.X = 2 * std::atan2(sinp, cosp) - Math::HalfPI;

		// Yaw (Y-axis rotation)
		double siny_cosp = 2 * (W * Y + Z * X);
		double cosy_cosp = 1 - 2 * (X * X + Y * Y);
		eulerAngles.Y = std::atan2(siny_cosp, cosy_cosp);

		return eulerAngles;
	}
}