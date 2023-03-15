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

	Matrix4x4::Matrix4x4() noexcept : Matrix4x4(false)
	{}

	Matrix4x4::Matrix4x4(bool isIdentity) noexcept :
		Data {0.0}
	{
		if (isIdentity)
		{
			Data[m11] = 1.0;
			Data[m22] = 1.0;
			Data[m33] = 1.0;
			Data[m44] = 1.0;
		}
	}

	Matrix4x4::Matrix4x4(const Array<double, CellCount>& data) noexcept
	{
		std::copy(data.begin(), data.end(), Data.begin());
	}

	Matrix4x4::Matrix4x4(const Matrix4x4& other) noexcept
	{
		std::copy(other.Data.begin(), other.Data.end(), Data.begin());
	}

	Matrix4x4::Matrix4x4(Matrix4x4&& other) noexcept :
		Data(std::move(other.Data))
	{}

	Matrix4x4& Matrix4x4::operator=(const Matrix4x4 & other) noexcept
	{
		std::copy(other.Data.begin(), other.Data.end(), Data.begin());
		return *this;
	}

	Matrix4x4& Matrix4x4::operator=(Matrix4x4&& other) noexcept
	{
		Data = std::move(other.Data);
		return *this;
	}

	Matrix4x4 Matrix4x4::CreateOrthographicProjection(double left, double right, double top, double bottom, double nearClip, double farClip) noexcept
	{
		Matrix4x4 ortho = Identity;

		const double rl = 1.0 / (right - left);
		const double tb = 1.0 / (top - bottom);
		const double fn = 1.0 / (farClip - nearClip);

		// This creates an orthographic matrix for Vulkan/DirectX with the directions: right = X, up = -Y, forward = Z
		ortho.Data[m11] = 2.0 * rl;
		ortho.Data[m22] = 2.0 * tb;
		ortho.Data[m33] = -2.0 * fn;

		ortho.Data[m14] = -(right + left) * rl;
		ortho.Data[m24] = -(top + bottom) * tb;
		ortho.Data[m34] = (farClip + nearClip) * fn;

		// TODO: how to handle OpenGL's flipped Y axis?
		return CocoViewToRenderView * ortho;
	}

	Matrix4x4 Matrix4x4::CreatePerspectiveProjection(double fieldOfViewRadians, double aspectRatio, double nearClip, double farClip) noexcept
	{
		Matrix4x4 perspective;

		const double oneOverHalfTanFOV = 1.0 / Math::Tan(fieldOfViewRadians * 0.5);

		// This creates a projection matrix for Vulkan/DirectX with the directions: right = X, up = -Y, forward = Z
		perspective.Data[m11] = oneOverHalfTanFOV / aspectRatio;
		perspective.Data[m22] = oneOverHalfTanFOV;
		
		perspective.Data[m33] = (farClip + nearClip) / (farClip - nearClip);
		perspective.Data[m34] = -2.0 * farClip * nearClip / (farClip - nearClip);
		
		perspective.Data[m43] = 1.0;
		perspective.Data[m44] = 0.0;

		// TODO: how to handle OpenGL's flipped Y axis?
		return CocoViewToRenderView * perspective;
	}

	Matrix4x4 Matrix4x4::CreateLookAtMatrix(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up) noexcept
	{
		Matrix4x4 lookAt = Identity;

		Vector3 yAxis = targetPosition - eyePosition;
		yAxis.Normalize();

		Vector3 xAxis = yAxis.Cross(up);
		xAxis.Normalize();

		Vector3 zAxis = xAxis.Cross(yAxis);

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

		return lookAt;
	}

	Matrix4x4 Matrix4x4::CreateWithTranslation(const Vector3& translation) noexcept
	{
		Matrix4x4 mat = Identity;
		mat.Data[m14] = translation.X;
		mat.Data[m24] = translation.Y;
		mat.Data[m34] = translation.Z;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithScale(const Vector3& scale) noexcept
	{
		Matrix4x4 mat = Identity;
		mat.Data[m11] = scale.X;
		mat.Data[m22] = scale.Y;
		mat.Data[m33] = scale.Z;

		return mat;
	}

	Matrix4x4 Matrix4x4::CreateWithRotationEulerX(double angleRadians) noexcept
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

	Matrix4x4 Matrix4x4::CreateWithRotationEulerY(double angleRadians) noexcept
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

	Matrix4x4 Matrix4x4::CreateWithRotationEulerZ(double angleRadians) noexcept
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

	Matrix4x4 Matrix4x4::CreateWithEulerRotation(double xRadians, double yRadians, double zRadians) noexcept
	{
		const Matrix4x4 rX = CreateWithRotationEulerX(xRadians);
		const Matrix4x4 rY = CreateWithRotationEulerY(yRadians);
		const Matrix4x4 rZ = CreateWithRotationEulerZ(zRadians);

		const Matrix4x4 mat = rX * rY;
		return mat * rZ;
	}

	Matrix4x4 Matrix4x4::CreateWithEulerRotation(const Vector3& eulerAnglesRadians) noexcept
	{
		return CreateWithEulerRotation(eulerAnglesRadians.X, eulerAnglesRadians.Y, eulerAnglesRadians.Z);
	}

	Matrix4x4 Matrix4x4::CreateTransform(const Vector3& position, const Quaternion& orientation, const Vector3& scale) noexcept
	{
		return Matrix4x4::CreateWithScale(scale) * orientation.ToRotationMatrix() * Matrix4x4::CreateWithTranslation(position);
	}

	Matrix4x4 Matrix4x4::Inverted() const noexcept
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

		double det =	Data[m11] * (Data[m22] * A2323 - Data[m23] * A1323 + Data[m24] * A1223) -
						Data[m12] * (Data[m21] * A2323 - Data[m23] * A0323 + Data[m24] * A0223) +
						Data[m13] * (Data[m21] * A1323 - Data[m22] * A0323 + Data[m24] * A0123) -
						Data[m14] * (Data[m21] * A1223 - Data[m22] * A0223 + Data[m23] * A0123);
		det = 1 / det;

		inverse.Data[m11] = det * (	Data[m22] * A2323 - Data[m23] * A1323 + Data[m24] * A1223);
		inverse.Data[m12] = det * -(Data[m12] * A2323 - Data[m13] * A1323 + Data[m14] * A1223);
		inverse.Data[m13] = det * (	Data[m12] * A2313 - Data[m13] * A1313 + Data[m14] * A1213);
		inverse.Data[m14] = det * -(Data[m12] * A2312 - Data[m13] * A1312 + Data[m14] * A1212);
		inverse.Data[m21] = det * -(Data[m21] * A2323 - Data[m23] * A0323 + Data[m24] * A0223);
		inverse.Data[m22] = det * (	Data[m11] * A2323 - Data[m13] * A0323 + Data[m14] * A0223);
		inverse.Data[m23] = det * -(Data[m11] * A2313 - Data[m13] * A0313 + Data[m14] * A0213);
		inverse.Data[m24] = det * (	Data[m11] * A2312 - Data[m13] * A0312 + Data[m14] * A0212);
		inverse.Data[m31] = det * (	Data[m21] * A1323 - Data[m22] * A0323 + Data[m24] * A0123);
		inverse.Data[m32] = det * -(Data[m11] * A1323 - Data[m12] * A0323 + Data[m14] * A0123);
		inverse.Data[m33] = det * (	Data[m11] * A1313 - Data[m12] * A0313 + Data[m14] * A0113);
		inverse.Data[m34] = det * -(Data[m11] * A1312 - Data[m12] * A0312 + Data[m14] * A0112);
		inverse.Data[m41] = det * -(Data[m21] * A1223 - Data[m22] * A0223 + Data[m23] * A0123);
		inverse.Data[m42] = det * (	Data[m11] * A1223 - Data[m12] * A0223 + Data[m13] * A0123);
		inverse.Data[m43] = det * -(Data[m11] * A1213 - Data[m12] * A0213 + Data[m13] * A0113);
		inverse.Data[m44] = det * (	Data[m11] * A1212 - Data[m12] * A0212 + Data[m13] * A0112);

		return inverse;
	}

	Matrix4x4 Matrix4x4::Transposed() const noexcept
	{
		Matrix4x4 mat = Identity;

		mat.Data[m11] =	Data[m11];
		mat.Data[m21] =	Data[m12];
		mat.Data[m31] =	Data[m13];
		mat.Data[m41] =	Data[m14];
		mat.Data[m12] =	Data[m21];
		mat.Data[m22] =	Data[m22];
		mat.Data[m32] =	Data[m23];
		mat.Data[m42] =	Data[m24];
		mat.Data[m13] =	Data[m31];
		mat.Data[m23] =	Data[m32];
		mat.Data[m33] =	Data[m33];
		mat.Data[m43] =	Data[m34];
		mat.Data[m14] =	Data[m41];
		mat.Data[m24] =	Data[m42];
		mat.Data[m34] =	Data[m43];
		mat.Data[m44] =	Data[m44];

		return mat;
	}

	Vector3 Matrix4x4::GetRightVector() const noexcept
	{
		Vector3 vec(Data[m11], Data[m21], Data[m31]);
		vec.Normalize();
		return vec;
	}

	Vector3 Matrix4x4::GetUpVector() const noexcept
	{
		Vector3 vec(Data[m13], Data[m23], Data[m33]);
		vec.Normalize();
		return vec;
	}

	Vector3 Matrix4x4::GetForwardVector() const noexcept
	{
		Vector3 vec(Data[m12], Data[m22], Data[m32]);
		vec.Normalize();
		return vec;
	}

	Array<float, Matrix4x4::CellCount> Matrix4x4::AsFloat() const noexcept
	{
		Array<float, Matrix4x4::CellCount> data = {};

		for (int i = 0; i < CellCount; i++)
			data.at(i) = static_cast<float>(Data.at(i));

		return std::move(data);
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const noexcept
	{
		Matrix4x4 result;

		size_t thisIndex = 0;
		size_t destIndex = 0;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.Data[destIndex] =	Data[thisIndex + 0] * other.Data[0 + static_cast<size_t>(j)] +
											Data[thisIndex + 1] * other.Data[4 + static_cast<size_t>(j)] +
											Data[thisIndex + 2] * other.Data[8 + static_cast<size_t>(j)] +
											Data[thisIndex + 3] * other.Data[12 + static_cast<size_t>(j)];

				destIndex++;
			}

			thisIndex += 4;
		}

		return result;
	}
}