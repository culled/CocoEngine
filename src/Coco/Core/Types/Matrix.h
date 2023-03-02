#pragma once

#include <Coco/Core/Core.h>

#include "Vector.h"

namespace Coco
{
	struct COCOAPI Matrix4x4
	{
		static const int CellCount = 16;
		static const Matrix4x4 Identity;

		double Data[CellCount];

		Matrix4x4();
		Matrix4x4(double data[CellCount]);

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
		Vector3 GetBackwardsVector() const;

		/// <summary>
		/// Gets a vector that points forwards
		/// </summary>
		/// <returns>A vector that points forwards</returns>
		Vector3 GetForwardsVector() const { return -GetBackwardsVector(); }

		Matrix4x4 operator*(const Matrix4x4& other) const;
	};
}