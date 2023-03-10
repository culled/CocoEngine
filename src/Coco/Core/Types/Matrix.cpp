#include "Matrix.h"
#include <Coco/Core/Math/Math.h>

namespace Coco
{
	const Matrix4x4 Matrix4x4::Identity = Matrix4x4(true);

	Matrix4x4::Matrix4x4(bool isIdentity)
	{
		for (int i = 0; i < CellCount; i++)
		{
			Data[i] = 0.0;

			if (isIdentity && (i == 0 || i == 5 || i == 10 || i == 15))
				Data[i] = 1.0;
		}
	}

	Matrix4x4::Matrix4x4(double data[CellCount])
	{
		for (int i = 0; i < CellCount; i++)
		{
			Data[i] = data[i];
		}
	}

	Matrix4x4::Matrix4x4(const Matrix4x4& other)
	{
		std::memcpy(Data, other.Data, CellCount * sizeof(double));
	}

	Matrix4x4 Matrix4x4::CreateOrthographicProjection(double left, double right, double top, double bottom, double nearClip, double farClip)
	{
		Matrix4x4 ortho = Identity;

		double lr = 1.0 / (left - right);
		double bt = 1.0 / (bottom - top);
		double nf = 1.0 / (nearClip - farClip);

		double* m = ortho.Data;
		m[0] = -2.0 * lr;
		m[5] = -2.0 * bt;
		m[10] = -2.0 * nf;

		m[12] = (left + right) * lr;
		m[13] = (top + bottom) * bt;
		m[14] = (farClip + nearClip) * nf;

		return ortho;
	}

	Matrix4x4 Matrix4x4::CreatePerspectiveProjection(double fieldOfViewRadians, double aspectRatio, double nearClip, double farClip)
	{
		Matrix4x4 perspective = Identity;

		double halfTanFOV = Math::Tan(fieldOfViewRadians * 0.5);

		double* m = perspective.Data;
		m[0] = 1.0 / (aspectRatio * halfTanFOV);
		m[5] = 1.0 / halfTanFOV;
		m[10] = -((farClip + nearClip) / (farClip - nearClip));
		m[11] = -1.0;
		m[14] = -((2.0 * farClip * nearClip) / (farClip - nearClip));

		return perspective;
	}

	Matrix4x4 Matrix4x4::CreateLookAtMatrix(const Vector3& position, const Vector3& targetPosition, const Vector3& up)
	{
		Matrix4x4 lookAt = Identity;

		Vector3 zAxis = targetPosition - position;
		zAxis.Normalize();

		Vector3 xAxis = zAxis.Cross(up);
		xAxis.Normalize();

		Vector3 yAxis = xAxis.Cross(zAxis);

		double* m = lookAt.Data;
		m[0] = xAxis.X;
		m[1] = yAxis.X;
		m[2] = -zAxis.X;
		m[3] = 0.0;

		m[4] = xAxis.Y;
		m[5] = yAxis.Y;
		m[6] = -zAxis.Y;
		m[7] = 0.0;

		m[8] = xAxis.Z;
		m[9] = yAxis.Z;
		m[10] = -zAxis.Z;
		m[11] = 0.0;

		m[12] = -xAxis.Dot(position);
		m[13] = -yAxis.Dot(position);
		m[14] = zAxis.Dot(position);
		m[15] = 1.0;

		return lookAt;
	}

	Matrix4x4 Matrix4x4::CreateWithTranslation(const Vector3& translation)
	{
		Matrix4x4 mat = Identity;
		mat.Data[12] = translation.X;
		mat.Data[13] = translation.Y;
		mat.Data[14] = translation.Z;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithScale(const Vector3& scale)
	{
		Matrix4x4 mat = Identity;
		mat.Data[0] = scale.X;
		mat.Data[5] = scale.Y;
		mat.Data[10] = scale.Z;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerX(double angleRadians)
	{
		Matrix4x4 mat = Identity;
		
		double c = Math::Cos(angleRadians);
		double s = Math::Sin(angleRadians);
		
		mat.Data[5] = c;
		mat.Data[6] = s;
		mat.Data[9] = -s;
		mat.Data[10] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerY(double angleRadians)
	{
		Matrix4x4 mat = Identity;

		double c = Math::Cos(angleRadians);
		double s = Math::Sin(angleRadians);

		mat.Data[0] = c;
		mat.Data[2] = -s;
		mat.Data[8] = s;
		mat.Data[10] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerZ(double angleRadians)
	{
		Matrix4x4 mat = Identity;

		double c = Math::Cos(angleRadians);
		double s = Math::Sin(angleRadians);

		mat.Data[0] = c;
		mat.Data[1] = s;
		mat.Data[4] = -s;
		mat.Data[5] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithEulerRotation(double xRadians, double yRadians, double zRadians)
	{
		Matrix4x4 rX = CreateWithRotationEulerX(xRadians);
		Matrix4x4 rY = CreateWithRotationEulerY(yRadians);
		Matrix4x4 rZ = CreateWithRotationEulerZ(zRadians);

		Matrix4x4 mat = rX * rY;
		return mat * rZ;
	}

	Matrix4x4 Matrix4x4::Inverted() const
	{
		const double* m = this->Data;

		double t0 = m[10] * m[15];
		double t1 = m[14] * m[11];
		double t2 = m[6] * m[15];
		double t3 = m[14] * m[7];
		double t4 = m[6] * m[11];
		double t5 = m[10] * m[7];
		double t6 = m[2] * m[15];
		double t7 = m[14] * m[3];
		double t8 = m[2] * m[11];
		double t9 = m[10] * m[3];
		double t10 = m[2] * m[7];
		double t11 = m[6] * m[3];
		double t12 = m[8] * m[13];
		double t13 = m[12] * m[9];
		double t14 = m[4] * m[13];
		double t15 = m[12] * m[5];
		double t16 = m[4] * m[9];
		double t17 = m[8] * m[5];
		double t18 = m[0] * m[13];
		double t19 = m[12] * m[1];
		double t20 = m[0] * m[9];
		double t21 = m[8] * m[1];
		double t22 = m[0] * m[5];
		double t23 = m[4] * m[1];

		Matrix4x4 inverse;

		double* o = inverse.Data;
		o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
		o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
		o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
		o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);

		double d = 1.0 / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);

		o[0] = d * o[0];
		o[1] = d * o[1];
		o[2] = d * o[2];
		o[3] = d * o[3];
		o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
		o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
		o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
		o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
		o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
		o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
		o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
		o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
		o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
		o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
		o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
		o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));

		return inverse;
	}

	Matrix4x4 Matrix4x4::Transposed() const
	{
		Matrix4x4 mat = Identity;

		double* o = mat.Data;
		const double* m = this->Data;

		o[0] = m[0];
		o[1] = m[4];
		o[2] = m[8];
		o[3] = m[12];
		o[4] = m[1];
		o[5] = m[5];
		o[6] = m[9];
		o[7] = m[13];
		o[8] = m[2];
		o[9] = m[6];
		o[10] = m[10];
		o[11] = m[14];
		o[12] = m[3];
		o[13] = m[7];
		o[14] = m[11];
		o[15] = m[15];

		return mat;
	}

	Vector3 Matrix4x4::GetRightVector() const
	{
		Vector3 vec(this->Data[0], this->Data[4], this->Data[8]);
		vec.Normalize();
		return vec;
	}

	Vector3 Matrix4x4::GetUpVector() const
	{
		Vector3 vec(this->Data[1], this->Data[5], this->Data[9]);
		vec.Normalize();
		return vec;
	}

	Vector3 Matrix4x4::GetBackwardsVector() const
	{
		Vector3 vec(this->Data[2], this->Data[6], this->Data[10]);
		vec.Normalize();
		return vec;
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
	{
		Matrix4x4 result = Identity;

		double* destPtr = result.Data;
		const double* thisPtr = this->Data;
		const double* otherPtr = other.Data;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				*destPtr =	thisPtr[0] * otherPtr[0 + j] +
							thisPtr[1] * otherPtr[4 + j] +
							thisPtr[2] * otherPtr[8 + j] +
							thisPtr[3] * otherPtr[12 + j];
				destPtr++;
			}

			thisPtr += 4;
		}

		return result;
	}
}