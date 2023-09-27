#include "Corepch.h"
#include "Quaternion.h"
#include "Vector.h"

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

		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToQuaternion/index.htm
		// Pitch (X), Roll (Z), Yaw (Y) - YXZ order
		const double c1 = Math::Cos(halfAngles.Y);
		const double s1 = Math::Sin(halfAngles.Y);
		const double c2 = Math::Cos(halfAngles.X);
		const double s2 = Math::Sin(halfAngles.X);
		const double c3 = Math::Cos(halfAngles.Z);
		const double s3 = Math::Sin(halfAngles.Z);

		W = c1 * c2 * c3 - -(s1 * s2) * s3;
		X = c1 * s2 * c3 + s1 * c2 * s3;
		Y = -(c1 * s2) * s3 + s1 * c2 * c3;
		Z = -(s1 * s2) * c3 + c1 * c2 * s3;
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
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
	
		double ww = W * W;
		double xx = X * X;
		double yy = Y * Y;
		double zz = Z * Z;
		double unit = xx + yy + zz + ww; // If normalised is one, otherwise is correction factor
		double test = X * W - Y * Z;

		Vector3 eulerAngles;

		const double unitTest = 0.5 - Math::Epsilon;
		if (test > unit * unitTest)
		{
			// Singularity at north pole
			//eulerAngles.Y = Math::Atan2(2.0 * (Y * W - X * Z), 1.0 - 2.0 * (yy + zz));
			eulerAngles.Y = Math::Atan2(2.0 * (Y * W - X * Z), xx - yy - zz + ww);
			eulerAngles.X = Math::HalfPI;
			eulerAngles.Z = 0.0;
		}
		else if (test < -unit * unitTest)
		{
			// Singularity at south pole
			eulerAngles.Y = Math::Atan2(2.0 * (Y * W - X * Z), xx - yy - zz + ww);
			eulerAngles.X = -Math::HalfPI;
			eulerAngles.Z = 0.0;
		}
		else
		{
			// No singularity
			eulerAngles.Y = Math::Atan2(2.0 * (X * Z + Y * W), ww - xx - yy + zz);
			eulerAngles.X = Math::Asin(-2.0 * (Y * Z - W * X));
			eulerAngles.Z = Math::Atan2(2.0 * (X * Y + W * Z), ww - xx + yy - zz);
		}

		return eulerAngles;
	}
}