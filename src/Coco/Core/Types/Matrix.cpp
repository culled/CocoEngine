#include "Corepch.h"
#include "Matrix.h"

#include "../Math/Math.h"

namespace Coco
{
	// Matrix indexing
	// [0	m11,	4	m12,	8	m13,	12	m14]
	// [1	m21,	5	m22,	9	m23,	13	m24]
	// [2	m31,	6	m32,	10	m33,	14	m34]
	// [3	m41,	7	m42,	11	m43,	15	m44]

	const Matrix4x4 Matrix4x4::Identity = Matrix4x4(true);

	Matrix4x4::Matrix4x4() : 
		Matrix4x4(false)
	{}

	Matrix4x4::Matrix4x4(bool isIdentity) :
		Data{ 0.0 }
	{
		if (isIdentity)
		{
			Data[m11] = 1.0;
			Data[m22] = 1.0;
			Data[m33] = 1.0;
			Data[m44] = 1.0;
		}
	}

	Matrix4x4::Matrix4x4(const DataType& data) :
		Data{data}
	{}

	Matrix4x4::Matrix4x4(const Matrix4x4& other) :
		Data{other.Data}
	{}

	Matrix4x4::Matrix4x4(Matrix4x4&& other) :
		Data(std::move(other.Data))
	{}

	Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other)
	{
		Data = other.Data;
		return *this;
	}

	Matrix4x4& Matrix4x4::operator=(Matrix4x4&& other)
	{
		Data = std::move(other.Data);
		return *this;
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
	{
		Matrix4x4 result;

		size_t thisIndex = 0;
		size_t destIndex = 0;

		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				result.Data[destIndex] =	Data[thisIndex + 0] * other.Data[0 + j] +
											Data[thisIndex + 1] * other.Data[4 + j] +
											Data[thisIndex + 2] * other.Data[8 + j] +
											Data[thisIndex + 3] * other.Data[12 + j];

				destIndex++;
			}

			thisIndex += 4;
		}

		return result;
	}

	Vector4 Matrix4x4::operator*(const Vector4& vector) const
	{
		return Vector4(
			vector.X * Data[m11] + vector.Y * Data[m12] + vector.Z * Data[m13] + vector.W * Data[m14],
			vector.X * Data[m21] + vector.Y * Data[m22] + vector.Z * Data[m23] + vector.W * Data[m24],
			vector.X * Data[m31] + vector.Y * Data[m32] + vector.Z * Data[m33] + vector.W * Data[m34],
			vector.X * Data[m41] + vector.Y * Data[m42] + vector.Z * Data[m43] + vector.W * Data[m44]);
	}

	Quaternion Matrix4x4::operator*(const Quaternion& rotation) const
	{
		Matrix4x4 rotMatrix = CreateWithRotation(rotation);
		return (*this * rotMatrix).GetRotation();
	}

	Matrix4x4 Matrix4x4::CreateLookAtMatrix(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up)
	{
		Matrix4x4 lookAt;

		Vector3 zAxis = targetPosition - eyePosition;
		zAxis.Normalize();

		Vector3 xAxis = up.Cross(zAxis);
		xAxis.Normalize();

		Vector3 yAxis = zAxis.Cross(xAxis);

		lookAt.Data[m11] = xAxis.X;
		lookAt.Data[m21] = yAxis.X;
		lookAt.Data[m31] = zAxis.X;

		lookAt.Data[m12] = xAxis.Y;
		lookAt.Data[m22] = yAxis.Y;
		lookAt.Data[m32] = zAxis.Y;

		lookAt.Data[m13] = xAxis.Z;
		lookAt.Data[m23] = yAxis.Z;
		lookAt.Data[m33] = zAxis.Z;

		lookAt.Data[m14] = -xAxis.Dot(eyePosition);
		lookAt.Data[m24] = -yAxis.Dot(eyePosition);
		lookAt.Data[m34] = -zAxis.Dot(eyePosition);
		lookAt.Data[m44] = 1.0;

		return lookAt;
	}

	Matrix4x4 Matrix4x4::CreateWithTranslation(const Vector3& translation)
	{
		Matrix4x4 mat = Identity;
		mat.Data[m14] = translation.X;
		mat.Data[m24] = translation.Y;
		mat.Data[m34] = translation.Z;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithScale(const Vector3& scale)
	{
		Matrix4x4 mat;
		mat.Data[m11] = scale.X;
		mat.Data[m22] = scale.Y;
		mat.Data[m33] = scale.Z;
		mat.Data[m44] = 1.0;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerX(double angleRadians)
	{
		Matrix4x4 mat = Identity;

		const double c = Math::Cos(angleRadians);
		const double s = Math::Sin(angleRadians);

		mat.Data[m22] = c;
		mat.Data[m32] = s;
		mat.Data[m23] = -s;
		mat.Data[m33] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerY(double angleRadians)
	{
		Matrix4x4 mat = Identity;

		const double c = Math::Cos(angleRadians);
		const double s = Math::Sin(angleRadians);

		mat.Data[m11] = c;
		mat.Data[m31] = -s;
		mat.Data[m13] = s;
		mat.Data[m33] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerZ(double angleRadians)
	{
		Matrix4x4 mat = Identity;

		const double c = Math::Cos(angleRadians);
		const double s = Math::Sin(angleRadians);

		mat.Data[m11] = c;
		mat.Data[m21] = s;
		mat.Data[m12] = -s;
		mat.Data[m22] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithEulerRotation(double xRadians, double yRadians, double zRadians)
	{
		return CreateWithRotationEulerX(xRadians) * CreateWithRotationEulerY(yRadians) * CreateWithRotationEulerZ(zRadians);
	}

	Matrix4x4 Matrix4x4::CreateWithEulerRotation(const Vector3& eulerAnglesRadians)
	{
		return CreateWithEulerRotation(eulerAnglesRadians.X, eulerAnglesRadians.Y, eulerAnglesRadians.Z);
	}

	Matrix4x4 Matrix4x4::CreateWithRotation(const Quaternion& rotation)
	{
		Matrix4x4 mat;

		const Quaternion q = rotation.Normalized();

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
		mat.Data[m11] = 1.0 - 2.0 * (yy + zz);
		mat.Data[m12] = 2.0 * (xy - zw);
		mat.Data[m13] = 2.0 * (xz + yw);

		mat.Data[m21] = 2.0 * (xy + zw);
		mat.Data[m22] = 1.0 - 2.0 * (xx + zz);
		mat.Data[m23] = 2.0 * (yz - xw);

		mat.Data[m31] = 2.0 * (xz - yw);
		mat.Data[m32] = 2.0 * (yz + xw);
		mat.Data[m33] = 1.0 - 2.0 * (xx + yy);

		mat.Data[m44] = 1.0;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
	{
		return Matrix4x4::CreateWithScale(scale) * Matrix4x4::CreateWithRotation(rotation) * Matrix4x4::CreateWithTranslation(position);
	}

	Matrix4x4 Matrix4x4::Inverted() const
	{
		Matrix4x4 inverse;

		// https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
		const double A2323 = Data[m33] * Data[m44] - Data[m34] * Data[m43];
		const double A1323 = Data[m32] * Data[m44] - Data[m34] * Data[m42];
		const double A1223 = Data[m32] * Data[m43] - Data[m33] * Data[m42];
		const double A0323 = Data[m31] * Data[m44] - Data[m34] * Data[m41];
		const double A0223 = Data[m31] * Data[m43] - Data[m33] * Data[m41];
		const double A0123 = Data[m31] * Data[m42] - Data[m32] * Data[m41];
		const double A2313 = Data[m23] * Data[m44] - Data[m24] * Data[m43];
		const double A1313 = Data[m22] * Data[m44] - Data[m24] * Data[m42];
		const double A1213 = Data[m22] * Data[m43] - Data[m23] * Data[m42];
		const double A2312 = Data[m23] * Data[m34] - Data[m24] * Data[m33];
		const double A1312 = Data[m22] * Data[m34] - Data[m24] * Data[m32];
		const double A1212 = Data[m22] * Data[m33] - Data[m23] * Data[m32];
		const double A0313 = Data[m21] * Data[m44] - Data[m24] * Data[m41];
		const double A0213 = Data[m21] * Data[m43] - Data[m23] * Data[m41];
		const double A0312 = Data[m21] * Data[m34] - Data[m24] * Data[m31];
		const double A0212 = Data[m21] * Data[m33] - Data[m23] * Data[m31];
		const double A0113 = Data[m21] * Data[m42] - Data[m22] * Data[m41];
		const double A0112 = Data[m21] * Data[m32] - Data[m22] * Data[m31];

		double det = Data[m11] * (Data[m22] * A2323 - Data[m23] * A1323 + Data[m24] * A1223) -
			Data[m12] * (Data[m21] * A2323 - Data[m23] * A0323 + Data[m24] * A0223) +
			Data[m13] * (Data[m21] * A1323 - Data[m22] * A0323 + Data[m24] * A0123) -
			Data[m14] * (Data[m21] * A1223 - Data[m22] * A0223 + Data[m23] * A0123);
		det = 1 / det;

		inverse.Data[m11] = det * (Data[m22] * A2323 - Data[m23] * A1323 + Data[m24] * A1223);
		inverse.Data[m12] = det * -(Data[m12] * A2323 - Data[m13] * A1323 + Data[m14] * A1223);
		inverse.Data[m13] = det * (Data[m12] * A2313 - Data[m13] * A1313 + Data[m14] * A1213);
		inverse.Data[m14] = det * -(Data[m12] * A2312 - Data[m13] * A1312 + Data[m14] * A1212);
		inverse.Data[m21] = det * -(Data[m21] * A2323 - Data[m23] * A0323 + Data[m24] * A0223);
		inverse.Data[m22] = det * (Data[m11] * A2323 - Data[m13] * A0323 + Data[m14] * A0223);
		inverse.Data[m23] = det * -(Data[m11] * A2313 - Data[m13] * A0313 + Data[m14] * A0213);
		inverse.Data[m24] = det * (Data[m11] * A2312 - Data[m13] * A0312 + Data[m14] * A0212);
		inverse.Data[m31] = det * (Data[m21] * A1323 - Data[m22] * A0323 + Data[m24] * A0123);
		inverse.Data[m32] = det * -(Data[m11] * A1323 - Data[m12] * A0323 + Data[m14] * A0123);
		inverse.Data[m33] = det * (Data[m11] * A1313 - Data[m12] * A0313 + Data[m14] * A0113);
		inverse.Data[m34] = det * -(Data[m11] * A1312 - Data[m12] * A0312 + Data[m14] * A0112);
		inverse.Data[m41] = det * -(Data[m21] * A1223 - Data[m22] * A0223 + Data[m23] * A0123);
		inverse.Data[m42] = det * (Data[m11] * A1223 - Data[m12] * A0223 + Data[m13] * A0123);
		inverse.Data[m43] = det * -(Data[m11] * A1213 - Data[m12] * A0213 + Data[m13] * A0113);
		inverse.Data[m44] = det * (Data[m11] * A1212 - Data[m12] * A0212 + Data[m13] * A0112);

		return inverse;
	}

	Quaternion Matrix4x4::GetRotation() const
	{
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
		const double t = Data[m11] + Data[m22] + Data[m33];
		Quaternion result;

		if (t > 0)
		{
			const double s = Math::Sqrt(t + 1.0) * 2.0;
			result.W = 0.25 * s;
			result.X = (Data[m32] - Data[m23]) / s;
			result.Y = (Data[m13] - Data[m31]) / s;
			result.Z = (Data[m21] - Data[m12]) / s;
		}
		else if (Data[m11] > Data[m22] && Data[m11] > Data[m33])
		{
			const double s = Math::Sqrt(1.0 + Data[m11] - Data[m22] - Data[m33]) * 2.0;
			result.W = (Data[m32] - Data[m23]) / s;
			result.X = 0.25 * s;
			result.Y = (Data[m12] - Data[m21]) / s;
			result.Z = (Data[m13] - Data[m31]) / s;
		}
		else if (Data[m22] > Data[m33])
		{
			const double s = Math::Sqrt(1.0 + Data[m22] - Data[m11] - Data[m33]) * 2.0;
			result.W = (Data[m13] - Data[m31]) / s;
			result.X = (Data[m12] - Data[m21]) / s;
			result.Y = 0.25 * s;
			result.Z = (Data[m23] - Data[m32]) / s;
		}
		else
		{
			const double s = Math::Sqrt(1.0 + Data[m22] - Data[m11] - Data[m33]) * 2.0;
			result.W = (Data[m21] - Data[m12]) / s;
			result.X = (Data[m13] - Data[m31]) / s;
			result.Y = (Data[m23] - Data[m32]) / s;
			result.Z = 0.25 * s;
		}

		return result;
	}

	Vector3 Matrix4x4::GetScale() const
	{
		return Vector3(
			Math::Sqrt(Data[m11] * Data[m11] + Data[m21] * Data[m21] + Data[m31] * Data[m31]),
			Math::Sqrt(Data[m12] * Data[m12] + Data[m22] * Data[m22] + Data[m32] * Data[m32]),
			Math::Sqrt(Data[m13] * Data[m13] + Data[m23] * Data[m23] + Data[m33] * Data[m33])
		);
	}

	std::array<float, Matrix4x4::CellCount> Matrix4x4::AsFloatArray() const
	{
		std::array<float, Matrix4x4::CellCount> values{};

		for (int i = 0; i < CellCount; i++)
			values[i] = static_cast<float>(Data[i]);

		return values;
	}
}