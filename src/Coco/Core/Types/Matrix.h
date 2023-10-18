#pragma once

#include "../Corepch.h"
#include "../Defines.h"

namespace Coco
{
	struct Vector3;
	struct Vector4;
	struct Quaternion;

	/// @brief A matrix with 4 rows and 4 columns, stored in column-major order
	struct Matrix4x4
	{
		/// @brief The number of cells in a 4x4 matrix
		static constexpr int CellCount = 4 * 4;

		/// @brief The type of data
		using DataType = std::array<double, CellCount>;

		// First row, first column
		static constexpr int m11 = 0;

		// Second row, first column
		static constexpr int m21 = 1;

		// Third row, first column
		static constexpr int m31 = 2;

		// Fourth row, first column
		static constexpr int m41 = 3;

		// First row, second column
		static constexpr int m12 = 4;

		// Second row, second column
		static constexpr int m22 = 5;

		// Third row, second column
		static constexpr int m32 = 6;

		// Fourth row, second column
		static constexpr int m42 = 7;

		// First row, third column
		static constexpr int m13 = 8;

		// Second row, third column
		static constexpr int m23 = 9;

		// Third row, third column
		static constexpr int m33 = 10;

		// Fourth row, third column
		static constexpr int m43 = 11;

		// First row, fourth column
		static constexpr int m14 = 12;

		// Second row, fourth column
		static constexpr int m24 = 13;

		// Third row, fourth column
		static constexpr int m34 = 14;

		// Fourth row, fourth column
		static constexpr int m44 = 15;

		/// @brief An identity matrix
		static const Matrix4x4 Identity;

		/// @brief The matrix data, stored in column-major order
		DataType Data;

		Matrix4x4();
		Matrix4x4(bool isIdentity);
		Matrix4x4(const DataType& data);
		Matrix4x4(const Matrix4x4& other);
		Matrix4x4(Matrix4x4&& other);

		Matrix4x4& operator=(const Matrix4x4& other);
		Matrix4x4& operator=(Matrix4x4&& other);

		Matrix4x4 operator*(const Matrix4x4& other) const;
		Vector4 operator*(const Vector4& vector) const;
		Quaternion operator*(const Quaternion& rotation) const;

		/// @brief Creates a view matrix that looks from an eye position at a target position
		/// @param eyePosition The eye position
		/// @param targetPosition The position to look at
		/// @param up The up direction
		/// @return A view matrix with the looking from the eye position to the target position
		static Matrix4x4 CreateLookAtMatrix(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up);

		/// @brief Creates a matrix with only a translation
		/// @param translation The translation
		/// @return A matrix
		static Matrix4x4 CreateWithTranslation(const Vector3& translation);

		/// @brief Creates a matrix with only a scale
		/// @param scale The scale
		/// @return A matrix
		static Matrix4x4 CreateWithScale(const Vector3& scale);

		/// @brief Creates a matrix with only a rotation on the X axis
		/// @param angleRadians The angle (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithRotationEulerX(double angleRadians);

		/// @brief Creates a matrix with only a rotation on the Y axis
		/// @param angleRadians The angle (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithRotationEulerY(double angleRadians);

		/// @brief Creates a matrix with only a rotation on the Z axis
		/// @param angleRadians The angle (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithRotationEulerZ(double angleRadians);

		/// @brief Creates a matrix with only a rotation
		/// @param xRadians The angle on the X axis (in radians)
		/// @param yRadians The angle on the Y axis (in radians)
		/// @param zRadians The angle on the Z axis (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithEulerRotation(double xRadians, double yRadians, double zRadians);

		/// @brief Creates a matrix with only a rotation
		/// @param eulerAnglesRadians The euler angles in radians
		/// @return A matrix
		static Matrix4x4 CreateWithEulerRotation(const Vector3& eulerAnglesRadians);

		/// @brief Creates a matrix with only a rotation
		/// @param rotation The rotation
		/// @return A matrix
		static Matrix4x4 CreateWithRotation(const Quaternion& rotation);

		/// @brief Creates a transform matrix with a translation, rotation, and scale
		/// @param position The position
		/// @param rotation The rotation
		/// @param scale The scale
		/// @return A transform matrix
		static Matrix4x4 CreateTransform(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

		/// @brief Multiplies this matrix with another
		/// @param other The other matrix
		/// @return The multiply result
		Matrix4x4 Mul(const Matrix4x4& other) const { return *this * other; }

		/// @brief Gets the determinant of this matrix
		/// @return The determinant
		double GetDeterminant() const;

		/// @brief Returns a matrix that is the inverse of this matrix
		/// @return The inverse of this matrix
		Matrix4x4 Inverted() const;

		/// @brief Gets a vector that points to the right
		/// @return A vector that points to the right
		Vector3 GetRightVector() const;

		/// @brief Gets a vector that points to the left
		/// @return A vector that points to the left
		Vector3 GetLeftVector() const;

		/// @brief Gets a vector that points upwards
		/// @return A vector that points upwards
		Vector3 GetUpVector() const;

		/// @brief Gets a vector that points downwards
		/// @return A vector that points downwards
		Vector3 GetDownVector() const;

		/// @brief Gets a vector that points forwards
		/// @return A vector that points forwards
		Vector3 GetForwardVector() const;

		/// @brief Gets a vector that points backwards
		/// @return A vector that points backwards
		Vector3 GetBackwardVector() const;

		/// @brief Gets the translation component of this matrix
		/// @return The translation component
		Vector3 GetTranslation() const;

		/// @brief Gets the rotation component of this matrix.
		/// NOTE: only works if this matrix has no scaling applied
		/// @return The rotation component
		Quaternion GetRotation() const;

		/// @brief Gets the absolute scale component of this matrix
		/// @return The absolute scale component
		Vector3 GetAbsoluteScale() const;

		/// @brief Decomposes this matrix into a translation, rotation, and scale
		/// @param outTranslation Will be set to the translation
		/// @param outRotation Will be set to the rotation
		/// @param outScale Will be set to the scale
		void Decompose(Vector3& outTranslation, Quaternion& outRotation, Vector3& outScale) const;

		/// @brief Gets this matrix's data as a float array of 16 elements
		/// @return This matrix's data as a float array of 16 elements
		std::array<float, Matrix4x4::CellCount> AsFloatArray() const;
	};
}