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

	Quaternion::Quaternion(const Vector3& eulerAngles, bool normalize) noexcept : Quaternion(Vector3::Up, eulerAngles.Z, normalize)
	{
		*this *= Quaternion(Vector3::Right, eulerAngles.X, normalize);
		*this *= Quaternion(Vector3::Forwards, eulerAngles.Y, normalize);	
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

		const double l = Normal();
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

		Quaternion q = Normalized();

		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		mat.Data[Matrix4x4::m11] = 1.0 - 2.0 * q.Y * q.Y - 2.0 * q.Z * q.Z;
		mat.Data[Matrix4x4::m21] = 2.0 * q.X * q.Y + 2.0 * q.Z * q.W;
		mat.Data[Matrix4x4::m31] = 2.0 * q.X * q.Z - 2.0 * q.Y * q.W;

		mat.Data[Matrix4x4::m12] = 2.0 * q.X * q.Y - 2.0 * q.Z * q.W;
		mat.Data[Matrix4x4::m22] = 1.0 - 2.0 * q.X * q.X - 2.0 * q.Z * q.Z;
		mat.Data[Matrix4x4::m32] = 2.0 * q.Y * q.Z + 2.0 * q.X * q.W;

		mat.Data[Matrix4x4::m13] = 2.0 * q.X * q.Z + 2.0 * q.Y * q.W;
		mat.Data[Matrix4x4::m23] = 2.0 * q.Y * q.Z - 2.0 * q.X * q.W;
		mat.Data[Matrix4x4::m33] = 1.0 - 2.0 * q.X * q.X - 2.0 * q.Y * q.Y;

		return mat;
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