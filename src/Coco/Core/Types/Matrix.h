#pragma once

#include <Coco/Core/API.h>

#include "Vector.h"
#include "Array.h"

namespace Coco
{
	struct Quaternion;

	/// @brief A matrix with 4 rows and 4 columns, stored in column-major order
	struct COCOAPI Matrix4x4
	{
		/// @brief The number of cells in a 4x4 matrix
		static constexpr int CellCount = 16;

		/// @brief An identity matrix
		static const Matrix4x4 Identity;

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

		/// @brief The matrix data, stored in column-major order
		Array<double, CellCount> Data = { 0.0 };

		Matrix4x4() noexcept = default;
		Matrix4x4(bool isIdentity) noexcept;
		Matrix4x4(const Array<double, CellCount>& data) noexcept;
		Matrix4x4(const Matrix4x4& other) noexcept;
		Matrix4x4(Matrix4x4&& other) noexcept;

		virtual ~Matrix4x4() = default;

		Matrix4x4& operator=(const Matrix4x4& other) noexcept;
		Matrix4x4& operator=(Matrix4x4&& other) noexcept;

		/// @brief Creates an orthographic projection matrix
		/// @param left The left side of the view frustrum
		/// @param right The right side of the view frustrum
		/// @param top The top side of the view frustrum
		/// @param bottom The bottom side of the view frustrum
		/// @param nearClip The distance to the near clipping plane
		/// @param farClip The distance to the far clipping plane
		/// @return An orthographic projection matrix
		static Matrix4x4 CreateOrthographicProjection(double left, double right, double top, double bottom, double nearClip, double farClip) noexcept;

		/// @brief Creates an orthographic projection matrix
		/// @param size The vertical size of the view frustrum
		/// @param aspectRatio The aspect ratio
		/// @param nearClip The distance to the near clipping plane
		/// @param farClip The distance to the far clipping plane
		/// @return An orthographic projection matrix
		static Matrix4x4 CreateOrthographicProjection(double size, double aspectRatio, double nearClip, double farClip) noexcept;

		/// @brief Creates a perspective matrix
		/// @param fieldOfViewRadians The vertical field of view (in radians)
		/// @param aspectRatio The aspect ratio
		/// @param nearClip The distance to the near clipping plane
		/// @param farClip The distance to the far clipping plane
		/// @return A perspective projection matrix
		static Matrix4x4 CreatePerspectiveProjection(double fieldOfViewRadians, double aspectRatio, double nearClip, double farClip) noexcept;

		/// @brief Creates a view matrix that looks from an eye position at a target position
		/// @param eyePosition The eye position
		/// @param targetPosition The position to look at
		/// @param up The up direction
		/// @return A view matrix with the looking from the eye position to the target position
		static Matrix4x4 CreateLookAtMatrix(const Vector3& eyePosition, const Vector3& targetPosition, const Vector3& up) noexcept;

		/// @brief Creates a matrix with only a translation
		/// @param translation The translation
		/// @return A matrix
		static Matrix4x4 CreateWithTranslation(const Vector3& translation) noexcept;

		/// @brief Creates a matrix with only a scale
		/// @param scale The scale
		/// @return A matrix
		static Matrix4x4 CreateWithScale(const Vector3& scale) noexcept;

		/// @brief Creates a matrix with only a rotation on the X axis
		/// @param angleRadians The angle (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithRotationEulerX(double angleRadians) noexcept;

		/// @brief Creates a matrix with only a rotation on the Y axis
		/// @param angleRadians The angle (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithRotationEulerY(double angleRadians) noexcept;

		/// @brief Creates a matrix with only a rotation on the Z axis
		/// @param angleRadians The angle (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithRotationEulerZ(double angleRadians) noexcept;

		/// @brief Creates a matrix with only a rotation
		/// @param xRadians The angle on the X axis (in radians)
		/// @param yRadians The angle on the Y axis (in radians)
		/// @param zRadians The angle on the Z axis (in radians)
		/// @return A matrix
		static Matrix4x4 CreateWithEulerRotation(double xRadians, double yRadians, double zRadians) noexcept;

		/// @brief Creates a matrix with only a rotation
		/// @param eulerAnglesRadians The euler angles in radians
		/// @return A matrix
		static Matrix4x4 CreateWithEulerRotation(const Vector3& eulerAnglesRadians) noexcept;

		/// @brief Creates a transform matrix with a translation, orientation, and scale
		/// @param position The position
		/// @param orientation The orientation
		/// @param scale The scale
		/// @return A transform matrix
		static Matrix4x4 CreateTransform(const Vector3& position, const Quaternion& orientation, const Vector3& scale) noexcept;

		/// @brief Multiplies the rows of this matrix with the columns of another matrix
		/// @param other The other matrix
		/// @return The multiply result
		Matrix4x4 Mul(const Matrix4x4& other) const noexcept { return *this * other; }

		/// @brief Returns a matrix that is the inverse of this matrix
		/// @return The inverse of this matrix
		Matrix4x4 Inverted() const noexcept;

		/// @brief Returns a matrix that is the transpose of this matrix (rows and colums swapped)
		/// @return The transpose of this matrix
		Matrix4x4 Transposed() const noexcept;

		/// @brief Gets a vector that points to the right
		/// @return A vector that points to the right
		Vector3 GetRightVector() const noexcept;

		/// @brief Gets a vector that points to the left
		/// @return A vector that points to the left
		Vector3 GetLeftVector() const noexcept { return -GetRightVector(); }

		/// @brief Gets a vector that points upwards
		/// @return A vector that points upwards
		Vector3 GetUpVector() const noexcept;

		/// @brief Gets a vector that points downwards
		/// @return A vector that points downwards
		Vector3 GetDownVector() const noexcept { return -GetUpVector(); }

		/// @brief Gets a vector that points forwards
		/// @return A vector that points forwards
		Vector3 GetForwardVector() const noexcept;

		/// @brief Gets a vector that points backwards
		/// @return A vector that points backwards
		Vector3 GetBackwardVector() const noexcept { return -GetForwardVector(); }

		/// @brief Gets this matrix's data as a float array of 16 elements
		/// @return This matrix's data as a float array of 16 elements
		Array<float, Matrix4x4::CellCount> AsFloat() const noexcept;

		Matrix4x4 operator*(const Matrix4x4& other) const noexcept;
	};
}