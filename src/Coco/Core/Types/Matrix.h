#pragma once

#include <Coco/Core/Core.h>

#include "Vector.h"
#include "Array.h"

namespace Coco
{
	struct Quaternion;

	/// <summary>
	/// A matrix with 4 rows and 4 columns, stored in column-major order
	/// </summary>
	struct COCOAPI Matrix4x4
	{
		static const int CellCount = 16;
		static const Matrix4x4 Identity;

		// First row, first column
		static const int m11 = 0;

		// Second row, first column
		static const int m21 = 1;

		// Third row, first column
		static const int m31 = 2;

		// Fourth row, first column
		static const int m41 = 3;

		// First row, second column
		static const int m12 = 4;

		// Second row, second column
		static const int m22 = 5;

		// Third row, second column
		static const int m32 = 6;

		// Fourth row, second column
		static const int m42 = 7;

		// First row, third column
		static const int m13 = 8;

		// Second row, third column
		static const int m23 = 9;

		// Third row, third column
		static const int m33 = 10;

		// Fourth row, third column
		static const int m43 = 11;

		// First row, fourth column
		static const int m14 = 12;

		// Second row, fourth column
		static const int m24 = 13;

		// Third row, fourth column
		static const int m34 = 14;

		// Fourth row, fourth column
		static const int m44 = 15;

		/// <summary>
		/// The matrix data, stored in column-major order
		/// </summary>
		double Data[CellCount];

		Matrix4x4(bool isIdentity = false);
		Matrix4x4(const Array<double, CellCount>& data);
		Matrix4x4(const Matrix4x4& other);

		/// <summary>
		/// Creates an orthographic projection matrix
		/// </summary>
		/// <param name="left">The left side of the view frustrum</param>
		/// <param name="right">The right side of the view frustrum</param>
		/// <param name="top">The top side of the view frustrum</param>
		/// <param name="bottom">The bottom side of the view frustrum</param>
		/// <param name="nearClip">The distance to the near clipping plane</param>
		/// <param name="farClip">The distance to the far clipping plane</param>
		/// <returns>An orthographic projection matrix</returns>
		static Matrix4x4 CreateOrthographicProjection(double left, double right, double top, double bottom, double nearClip, double farClip);

		/// <summary>
		/// Creates a perspective matrix
		/// </summary>
		/// <param name="fieldOfViewRadians">The field of view (in radians)</param>
		/// <param name="aspectRatio">The aspect ratio</param>
		/// <param name="nearClip">The distance to the near clipping plane</param>
		/// <param name="farClip">The distance to the far clipping plane</param>
		/// <returns>A perspective matrix</returns>
		static Matrix4x4 CreatePerspectiveProjection(double fieldOfViewRadians, double aspectRatio, double nearClip, double farClip);

		/// <summary>
		/// Creates a matrix that looks from a position to a target position
		/// </summary>
		/// <param name="position">The position</param>
		/// <param name="targetPosition">The position to look at</param>
		/// <param name="up">The up direction</param>
		/// <returns>A look at matrix</returns>
		static Matrix4x4 CreateLookAtMatrix(const Vector3& position, const Vector3& targetPosition, const Vector3& up);

		/// <summary>
		/// Creates a matrix with only a translation
		/// </summary>
		/// <param name="position">The translation</param>
		/// <returns>The matrix</returns>
		static Matrix4x4 CreateWithTranslation(const Vector3& translation);

		/// <summary>
		/// Creates a matrix with only a scale
		/// </summary>
		/// <param name="scale">The scale</param>
		/// <returns>The matrix</returns>
		static Matrix4x4 CreateWithScale(const Vector3& scale);

		/// <summary>
		/// Creates a matrix with only a rotation on the X axis
		/// </summary>
		/// <param name="angleRadians">The angle in radians</param>
		/// <returns>The matrix</returns>
		static Matrix4x4 CreateWithRotationEulerX(double angleRadians);

		/// <summary>
		/// Creates a matrix with only a rotation on the Y axis
		/// </summary>
		/// <param name="angleRadians">The angle in radians</param>
		/// <returns>The matrix</returns>
		static Matrix4x4 CreateWithRotationEulerY(double angleRadians);

		/// <summary>
		/// Creates a matrix with only a rotation on the Z axis
		/// </summary>
		/// <param name="angleRadians">The angle in radians</param>
		/// <returns>The matrix</returns>
		static Matrix4x4 CreateWithRotationEulerZ(double angleRadians);

		/// <summary>
		/// Creates a matrix with only a rotation
		/// </summary>
		/// <param name="xRadians">The angle in radians on the X axis</param>
		/// <param name="yRadians">The angle in radians on the Y axis</param>
		/// <param name="zRadians">The angle in radians on the Z axis</param>
		/// <returns>The matrix</returns>
		static Matrix4x4 CreateWithEulerRotation(double xRadians, double yRadians, double zRadians);

		/// <summary>
		/// Creates a matrix with only a rotation
		/// </summary>
		/// <param name="eulerAnglesRadians">The euler angles in radians</param>
		/// <returns>The matrix</returns>
		static Matrix4x4 CreateWithEulerRotation(const Vector3& eulerAnglesRadians);

		/// <summary>
		/// Creates a transform matrix with a translation, orientation, and scale
		/// </summary>
		/// <param name="position">The position</param>
		/// <param name="orientation">The orientation</param>
		/// <param name="scale">The scale</param>
		/// <returns>A transform matrix</returns>
		static Matrix4x4 CreateTransform(const Vector3& position, const Quaternion& orientation, const Vector3& scale);

		/// <summary>
		/// Multiplies the rows of this matrix with the columns of another matrix
		/// </summary>
		/// <param name="other">The other matrix</param>
		/// <returns>The multiply result</returns>
		Matrix4x4 Mul(const Matrix4x4& other) const { return *this * other; }

		/// <summary>
		/// Returns a matrix that is the inverse of this matrix
		/// </summary>
		/// <returns>The inverse of this matrix</returns>
		Matrix4x4 Inverted() const;

		/// <summary>
		/// Returns a matrix that is the transpose of this matrix (rows and colums swapped)
		/// </summary>
		/// <returns>The transpose of this matrix</returns>
		Matrix4x4 Transposed() const;

		/// <summary>
		/// Gets a vector that points to the right
		/// </summary>
		/// <returns>A vector that points to the right</returns>
		Vector3 GetRightVector() const;

		/// <summary>
		/// Gets a vector that points to the left
		/// </summary>
		/// <returns>A vector that points to the left</returns>
		Vector3 GetLeftVector() const { return -GetRightVector(); }

		/// <summary>
		/// Gets a vector that points upwards
		/// </summary>
		/// <returns>A vector that points upwards</returns>
		Vector3 GetUpVector() const;

		/// <summary>
		/// Gets a vector that points downwards
		/// </summary>
		/// <returns>A vector that points downwards</returns>
		Vector3 GetDownVector() const { return -GetUpVector(); }

		/// <summary>
		/// Gets a vector that points backwards
		/// </summary>
		/// <returns>A vector that points backwards</returns>
		Vector3 GetForwardVector() const;

		/// <summary>
		/// Gets a vector that points forwards
		/// </summary>
		/// <returns>A vector that points forwards</returns>
		Vector3 GetBackwardVector() const { return -GetForwardVector(); }

		/// <summary>
		/// Gets this matrix's data as a float array of 16 elements
		/// </summary>
		/// <returns>This matrix's data as a float array of 16 elements</returns>
		Array<float, Matrix4x4::CellCount> AsFloat() const;

		Matrix4x4 operator*(const Matrix4x4& other) const;
	};
}