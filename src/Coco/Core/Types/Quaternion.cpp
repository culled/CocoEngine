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

	Quaternion::Quaternion(const Vector3 & axis, double angleRadians, bool normalize)
	{
		const double halfAngle = angleRadians * 0.5;
		const double s = Math::Sin(halfAngle);
		const double c = Math::Cos(halfAngle);

		X = s * axis.X;
		Y = s * axis.Y;
		Z = s * axis.Z;
		W = c;

		if (normalize)
			Normalize();
	}

	double Quaternion::Dot(const Quaternion& a, const Quaternion& b)
	{
		return a.Dot(b);
	}

	Matrix4x4 Quaternion::CreateRotationMatrix(const Quaternion& rotation, const Vector3& center)
	{
		Matrix4x4 mat;

		double* m = mat.Data;
		m[0] = rotation.X * rotation.X - rotation.Y * rotation.Y - rotation.Z * rotation.Z + rotation.W * rotation.W;
		m[1] = 2.0 * (rotation.X * rotation.Y + rotation.Z * rotation.W);
		m[2] = 2.0 * (rotation.X * rotation.Z - rotation.Y * rotation.W);
		m[3] = center.X - center.X * m[0] - center.Y * m[1] - center.Z * m[2];

		m[4] = 2.0 * (rotation.X * rotation.Y - rotation.Z * rotation.W);
		m[5] = -(rotation.X * rotation.X) + rotation.Y * rotation.Y - rotation.Z * rotation.Z + rotation.W * rotation.W;
		m[6] = 2.0 * (rotation.Y * rotation.Z + rotation.X * rotation.W);
		m[7] = center.Y - center.X * m[4] - center.Y * m[5] - center.Z * m[6];

		m[8] = 2.0 * (rotation.X * rotation.Z + rotation.Y * rotation.W);
		m[9] = 2.0 * (rotation.Y * rotation.Z - rotation.X * rotation.W);
		m[10] = -(rotation.X * rotation.X) - rotation.Y * rotation.Y + rotation.Z * rotation.Z + rotation.W * rotation.W;
		m[11] = center.Z - center.X * m[8] - center.Y * m[9] - center.Z * m[10];

		m[12] = 0.0;
		m[13] = 0.0;
		m[14] = 0.0;
		m[15] = 1.0;

		return mat;
	}

	Quaternion Quaternion::Slerp(const Quaternion& from, const Quaternion& to, double alpha)
	{
		Quaternion out;

		Quaternion a = from.Normalized();
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
		const double dotThreshold = 0.9995;
		if (dot > dotThreshold)
		{
			out.X = a.X + (b.X - a.X) * alpha;
			out.Y = a.Y + (b.Y - a.Y) * alpha;
			out.Z = a.Z + (b.Z - a.Z) * alpha;
			out.W = a.W + (b.W - a.W) * alpha;
		}
		else
		{
			const double theta0 = Math::ACos(dot);
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

	double Quaternion::Normal() const
	{

		return Math::Sqrt(X * X + Y * Y + Z * Z + W * W);
	}

	void Quaternion::Normalize(bool safe)
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

	Quaternion Quaternion::Normalized(bool safe) const
	{
		Quaternion copy = *this;
		copy.Normalize(safe);
		return copy;
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
		return X * other.X +
			Y * other.Y +
			Z * other.Z +
			W * other.W;
	}

	Matrix4x4 Quaternion::ToMatrix4x4() const
	{
		Matrix4x4 mat = Matrix4x4::Identity;

		Quaternion q = Normalized();

		double* m = mat.Data;

		m[0] = 1.0 - 2.0 * q.Y * q.Y - 2.0 * q.Z * q.Z;
		m[1] = 2.0 * q.X * q.Y - 2.0 * q.Z * q.W;
		m[2] = 2.0 * q.X * q.Z + 2.0 * q.Y * q.W;

		m[4] = 2.0 * q.X * q.Y + 2.0 * q.Z * q.W;
		m[5] = 1.0 - 2.0 * q.X * q.X - 2.0 * q.Z * q.Z;
		m[6] = 2.0 * q.Y * q.Z - 2.0 * q.X * q.W;

		m[8] = 2.0 * q.X * q.Z - 2.0 * q.Y * q.W;
		m[9] = 2.0 * q.Y * q.Z + 2.0 * q.X * q.W;
		m[10] = 1.0 - 2.0 * q.X * q.X - 2.0 * q.Y * q.Y;

		return mat;
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