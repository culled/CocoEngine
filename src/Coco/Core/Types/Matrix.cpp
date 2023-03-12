#include "Matrix.h"

#include "Quaternion.h"

namespace Coco
{
	const Matrix4x4 Matrix4x4::Identity = Matrix4x4(true);

	// Multiplying this by a projection matrix will give a projection matrix that transforms 
	// Coco view space (forward on Y, up on Z, right on X) to Vulkan/DirectX view space (forward on Z, up on -Y, right on X)
	const Matrix4x4 CocoViewToRenderView({
			1.0, 0.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, -1.0, 0.0, 0.0,
			0.0, 0.0, 0.0, 1.0 });
	
	// Matrix indexing
	// [0	m11,	4	m12,	8	m13,	12	m14]
	// [1	m21,	5	m22,	9	m23,	13	m24]
	// [2	m31,	6	m32,	10	m33,	14	m34]
	// [3	m41,	7	m42,	11	m43,	15	m44]

	Matrix4x4::Matrix4x4(bool isIdentity)
	{
		for (int i = 0; i < CellCount; i++)
		{
			Data[i] = 0.0;

			if (isIdentity && (i == 0 || i == 5 || i == 10 || i == 15))
				Data[i] = 1.0;
		}
	}

	Matrix4x4::Matrix4x4(const Array<double, CellCount>& data)
	{
		std::memcpy(Data, data.data(), CellCount * sizeof(double));
	}

	Matrix4x4::Matrix4x4(const Matrix4x4& other)
	{
		std::memcpy(Data, other.Data, CellCount * sizeof(double));
	}

	Matrix4x4 Matrix4x4::CreateOrthographicProjection(double left, double right, double top, double bottom, double nearClip, double farClip)
	{
		Matrix4x4 ortho = Identity;

		double* m = ortho.Data;

		const double rl = 1.0 / (right - left);
		const double tb = 1.0 / (top - bottom);
		const double fn = 1.0 / (farClip - nearClip);

		// This creates an orthographic matrix for Vulkan/DirectX with the directions: right = X, up = -Y, forward = Z
		m[m11] = 2.0 * rl;
		m[m22] = 2.0 * tb;
		m[m33] = -2.0 * fn;

		m[m14] = -(right + left) * rl;
		m[m24] = -(top + bottom) * tb;
		m[m34] = (farClip + nearClip) * fn;

		// TODO: how to handle OpenGL's flipped Y axis?
		return CocoViewToRenderView * ortho;
	}

	Matrix4x4 Matrix4x4::CreatePerspectiveProjection(double fieldOfViewRadians, double aspectRatio, double nearClip, double farClip)
	{
		Matrix4x4 perspective;

		const double oneOverHalfTanFOV = 1.0 / Math::Tan(fieldOfViewRadians * 0.5);

		double* m = perspective.Data;

		// This creates a projection matrix for Vulkan/DirectX with the directions: right = X, up = -Y, forward = Z
		m[m11] = oneOverHalfTanFOV / aspectRatio;
		m[m22] = oneOverHalfTanFOV;
		
		m[m33] = (farClip + nearClip) / (farClip - nearClip);
		m[m34] = -2.0 * farClip * nearClip / (farClip - nearClip);
		
		m[m43] = 1.0;
		m[m44] = 0.0;

		// TODO: how to handle OpenGL's flipped Y axis?
		return CocoViewToRenderView * perspective;
	}

	Matrix4x4 Matrix4x4::CreateLookAtMatrix(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up)
	{
		Matrix4x4 lookAt = Identity;

		Vector3 yAxis = targetPosition - eyePosition;
		yAxis.Normalize();

		Vector3 xAxis = yAxis.Cross(up);
		xAxis.Normalize();

		Vector3 zAxis = xAxis.Cross(yAxis);

		double* m = lookAt.Data;
		m[m11] = xAxis.X;
		m[m21] = yAxis.X;
		m[m31] = zAxis.X;
		
		m[m12] = xAxis.Y;
		m[m22] = yAxis.Y;
		m[m32] = zAxis.Y;
		
		m[m13] = xAxis.Z;
		m[m23] = yAxis.Z;
		m[m33] = zAxis.Z;
		
		m[m14] = -xAxis.Dot(eyePosition);
		m[m24] = -yAxis.Dot(eyePosition);
		m[m34] = -zAxis.Dot(eyePosition);

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
		Matrix4x4 mat = Identity;
		mat.Data[m11] = scale.X;
		mat.Data[m22] = scale.Y;
		mat.Data[m33] = scale.Z;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerX(double angleRadians)
	{
		Matrix4x4 mat = Identity;
		
		double c = Math::Cos(angleRadians);
		double s = Math::Sin(angleRadians);
		
		mat.Data[m22] = c;
		mat.Data[m32] = s;
		mat.Data[m23] = -s;
		mat.Data[m33] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerY(double angleRadians)
	{
		Matrix4x4 mat = Identity;

		double c = Math::Cos(angleRadians);
		double s = Math::Sin(angleRadians);

		mat.Data[m11] = c;
		mat.Data[m31] = -s;
		mat.Data[m13] = s;
		mat.Data[m33] = c;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerZ(double angleRadians)
	{
		Matrix4x4 mat = Identity;

		double c = Math::Cos(angleRadians);
		double s = Math::Sin(angleRadians);

		mat.Data[m11] = c;
		mat.Data[m21] = s;
		mat.Data[m12] = -s;
		mat.Data[m22] = c;

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

	Matrix4x4 Matrix4x4::CreateWithEulerRotation(const Vector3& eulerAnglesRadians)
	{
		return CreateWithEulerRotation(eulerAnglesRadians.X, eulerAnglesRadians.Y, eulerAnglesRadians.Z);
	}

	Matrix4x4 Matrix4x4::CreateTransform(const Vector3& position, const Quaternion& orientation, const Vector3& scale)
	{
		return Matrix4x4::CreateWithScale(scale) * orientation.ToRotationMatrix() * Matrix4x4::CreateWithTranslation(position);
	}

	Matrix4x4 Matrix4x4::Inverted() const
	{
		const double* m = Data;
		Matrix4x4 inverse;
		double* o = inverse.Data;

		// https://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
		const double A2323 = m[m33] * m[m44] - m[m34] * m[m43];
		const double A1323 = m[m32] * m[m44] - m[m34] * m[m42];
		const double A1223 = m[m32] * m[m43] - m[m33] * m[m42];
		const double A0323 = m[m31] * m[m44] - m[m34] * m[m41];
		const double A0223 = m[m31] * m[m43] - m[m33] * m[m41];
		const double A0123 = m[m31] * m[m42] - m[m32] * m[m41];
		const double A2313 = m[m23] * m[m44] - m[m24] * m[m43];
		const double A1313 = m[m22] * m[m44] - m[m24] * m[m42];
		const double A1213 = m[m22] * m[m43] - m[m23] * m[m42];
		const double A2312 = m[m23] * m[m34] - m[m24] * m[m33];
		const double A1312 = m[m22] * m[m34] - m[m24] * m[m32];
		const double A1212 = m[m22] * m[m33] - m[m23] * m[m32];
		const double A0313 = m[m21] * m[m44] - m[m24] * m[m41];
		const double A0213 = m[m21] * m[m43] - m[m23] * m[m41];
		const double A0312 = m[m21] * m[m34] - m[m24] * m[m31];
		const double A0212 = m[m21] * m[m33] - m[m23] * m[m31];
		const double A0113 = m[m21] * m[m42] - m[m22] * m[m41];
		const double A0112 = m[m21] * m[m32] - m[m22] * m[m31];

		double det = m[m11] * (m[m22] * A2323 - m[m23] * A1323 + m[m24] * A1223)
			- m[m12] * (m[m21] * A2323 - m[m23] * A0323 + m[m24] * A0223)
			+ m[m13] * (m[m21] * A1323 - m[m22] * A0323 + m[m24] * A0123)
			- m[m14] * (m[m21] * A1223 - m[m22] * A0223 + m[m23] * A0123);
		det = 1 / det;

		o[m11] = det * (m[m22] * A2323 - m[m23] * A1323 + m[m24] * A1223);
		o[m12] = det * -(m[m12] * A2323 - m[m13] * A1323 + m[m14] * A1223);
		o[m13] = det * (m[m12] * A2313 - m[m13] * A1313 + m[m14] * A1213);
		o[m14] = det * -(m[m12] * A2312 - m[m13] * A1312 + m[m14] * A1212);
		o[m21] = det * -(m[m21] * A2323 - m[m23] * A0323 + m[m24] * A0223);
		o[m22] = det * (m[m11] * A2323 - m[m13] * A0323 + m[m14] * A0223);
		o[m23] = det * -(m[m11] * A2313 - m[m13] * A0313 + m[m14] * A0213);
		o[m24] = det * (m[m11] * A2312 - m[m13] * A0312 + m[m14] * A0212);
		o[m31] = det * (m[m21] * A1323 - m[m22] * A0323 + m[m24] * A0123);
		o[m32] = det * -(m[m11] * A1323 - m[m12] * A0323 + m[m14] * A0123);
		o[m33] = det * (m[m11] * A1313 - m[m12] * A0313 + m[m14] * A0113);
		o[m34] = det * -(m[m11] * A1312 - m[m12] * A0312 + m[m14] * A0112);
		o[m41] = det * -(m[m21] * A1223 - m[m22] * A0223 + m[m23] * A0123);
		o[m42] = det * (m[m11] * A1223 - m[m12] * A0223 + m[m13] * A0123);
		o[m43] = det * -(m[m11] * A1213 - m[m12] * A0213 + m[m13] * A0113);
		o[m44] = det * (m[m11] * A1212 - m[m12] * A0212 + m[m13] * A0112);

		return inverse;
	}

	Matrix4x4 Matrix4x4::Transposed() const
	{
		Matrix4x4 mat = Identity;

		double* o = mat.Data;
		const double* m = Data;

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
		Vector3 vec(Data[m11], Data[m21], Data[m31]);
		vec.Normalize();
		return vec;
	}

	Vector3 Matrix4x4::GetUpVector() const
	{
		Vector3 vec(Data[m13], Data[m23], Data[m33]);
		vec.Normalize();
		return vec;
	}

	Vector3 Matrix4x4::GetForwardVector() const
	{
		Vector3 vec(Data[m12], Data[m22], Data[m32]);
		vec.Normalize();
		return vec;
	}

	Array<float, Matrix4x4::CellCount> Matrix4x4::AsFloat() const
	{
		Array<float, Matrix4x4::CellCount> data = {};

		for (int i = 0; i < CellCount; i++)
			data[i] = static_cast<float>(Data[i]);

		return std::move(data);
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