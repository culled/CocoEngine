#include "Quaternion.h"

#include "Matrix.h"
#include "Vector.h"

namespace Coco
{
	const Quaternion Quaternion::Identity = Quaternion(0.0, 0.0, 0.0, 1.0);

	Quaternion::Quaternion(double x, double y, double z, double w) noexcept :
		X(x), Y(y), Z(z), W(w)
	{}

	Quaternion::Quaternion(const Vector3& axis, double angleRadians, bool normalize) noexcept
	{
		const double halfAngle = angleRadians * 0.5;
		const double s = Math::Sin(halfAngle);
		const double c = Math::Cos(halfAngle);
		const Vector3 normalizedAxis = axis.Normalized();

		X = s * normalizedAxis.X;
		Y = s * normalizedAxis.Y;
		Z = s * normalizedAxis.Z;
		W = c;

		if (normalize)
			Normalize();
	}

	Quaternion::Quaternion(const Vector3& eulerAngles, bool normalize) noexcept
	{
		const Vector3 halfAngles = eulerAngles * 0.5;

		// Pitch (X), Roll (Y), Yaw (Z)
		const double cx = Math::Cos(halfAngles.X);
		const double sx = Math::Sin(halfAngles.X);
		const double cy = Math::Cos(halfAngles.Y);
		const double sy = Math::Sin(halfAngles.Y);
		const double cz = Math::Cos(halfAngles.Z);
		const double sz = Math::Sin(halfAngles.Z);

		W = (cz * cx * cy) - (sz * sx * sy);
		X = (cz * sx * cy) - (sz * cx * sy);
		Y = (sz * sx * cy) + (cz * cx * sy);
		Z = (cz * sx * sy) + (sz * cx * cy);
	}

	Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, double alpha) noexcept
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

	void Quaternion::Normalize(bool safe) noexcept
	{
		if (safe &&
			Math::Approximately(X, 0.0) &&
			Math::Approximately(Y, 0.0) &&
			Math::Approximately(Z, 0.0) &&
			Math::Approximately(W, 0.0))
			return;

		const double l = GetNormal();
		X /= l;
		Y /= l;
		Z /= l;
		W /= l;
	}

	Quaternion Quaternion::Normalized(bool safe) const noexcept
	{
		Quaternion copy = *this;
		copy.Normalize(safe);
		return copy;
	}

	Quaternion Quaternion::Inverted() const noexcept
	{
		Quaternion inv = Conjugate();
		inv.Normalize();
		return inv;
	}

	double Quaternion::Dot(const Quaternion& other) const noexcept
	{
		return X * other.X +
			Y * other.Y +
			Z * other.Z +
			W * other.W;
	}

	Matrix4x4 Quaternion::ToRotationMatrix() const noexcept
	{
		Matrix4x4 mat = Matrix4x4::Identity;

		const Quaternion q = Normalized();

		const double xx = q.X * q.X;
		const double xy = q.X * q.Y;
		const double xz = q.X * q.Z;
		const double xw = q.X * q.W;

		const double yy = q.Y * q.Y;
		const double yz = q.Y * q.Z;
		const double yw = q.Y * q.W;

		const double zz = q.Z * q.Z;
		const double zw = q.Z * q.W;

		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		mat.Data[Matrix4x4::m11] = 1.0 - 2.0 * (yy + zz);
		mat.Data[Matrix4x4::m12] = 2.0 * (xy - zw);
		mat.Data[Matrix4x4::m13] = 2.0 * (xz + yw);

		mat.Data[Matrix4x4::m21] = 2.0 * (xy + zw);
		mat.Data[Matrix4x4::m22] = 1.0 - 2.0 * (xx + zz);
		mat.Data[Matrix4x4::m23] = 2.0 * (yz - xw);

		mat.Data[Matrix4x4::m31] = 2.0 * (xz - yw);
		mat.Data[Matrix4x4::m32] = 2.0 * (yz + xw);
		mat.Data[Matrix4x4::m33] = 1.0 - 2.0 * (xx + yy);

		return mat;
	}

	Vector3 Quaternion::ToEulerAngles() const noexcept
	{
		Vector3 eulerAngles;

		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		const double w2 = W * W;
		const double x2 = X * X;
		const double y2 = Y * Y;
		const double z2 = Z * Z;
		const double unitLength = w2 + x2 + y2 + z2;
		const double abcd = W * X + Y * Z;

		if (abcd > (0.5 - Math::Epsilon) * unitLength)
		{
			eulerAngles.Z = 2.0 * Math::Atan2(Y, W);
			eulerAngles.X = Math::PI * 0.5;
			eulerAngles.Y = 0.0;
		}
		else if (abcd < (-0.5 + Math::Epsilon) * unitLength)
		{
			eulerAngles.Z = -2.0 * Math::Atan2(Y, W);
			eulerAngles.X = -Math::PI * 0.5;
			eulerAngles.Y = 0.0;
		}
		else
		{
			const double adbc = W * Z - X * Y;
			const double acbd = W * Y - X * Z;
			eulerAngles.Z = Math::Atan2(2.0 * adbc, 1.0 - 2.0 * (z2 + x2));
			eulerAngles.X = Math::Asin(2.0 * abcd / unitLength);
			eulerAngles.Y = Math::Atan2(2.0 * acbd, 1.0 - 2.0 * (y2 + x2));
		}

		return eulerAngles;
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const noexcept
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

	Vector3 Quaternion::operator*(const Vector3& direction) const noexcept
	{
		// https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		const Vector3 u(X, Y, Z);
		
		return u * 2.0 * u.Dot(direction) +
			direction * (W * W - u.GetLengthSquared()) +
			u.Cross(direction) * 2.0 * W;
	}
}