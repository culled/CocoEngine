//
// Created by cullen on 3/26/26.
//

#ifndef COCOENGINE_MATRIX4X4_H
#define COCOENGINE_MATRIX4X4_H
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

#include "Coco/Core/Types/Span.h"

#include <cstring>

#include "Coco/Core/Asserts.h"

namespace Coco
{
    /// @brief Base class for a row-major matrix with four rows and four columns
    /// @tparam ValueType The value type
    template<typename ValueType>
    struct BaseMatrix4x4
    {
        union
        {
            /// @brief The values accessible in a multi-dimensional array
            ValueType Values[4][4];

            /// @brief The values accessible by row
            BaseVector4<ValueType> Row[4];

            /// @brief The values accessible as a single array. Values are stored row-major, so the values in each row will be adjacent to each other
            ValueType Raw[16];
        };

        /// @brief The identity matrix
        static const BaseMatrix4x4 Identity;

        BaseMatrix4x4() :
            Raw{static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0),
            static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0),
            static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0),
            static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0)}
        {}

        BaseMatrix4x4(const BaseVector4<ValueType>& r1, const BaseVector4<ValueType>& r2, const BaseVector4<ValueType>& r3, const BaseVector4<ValueType>& r4) :
            Raw{r1.X(), r1.Y(), r1.Z(), r1.W(),
            r2.X(), r2.Y(), r2.Z(), r2.W(),
            r3.X(), r3.Y(), r3.Z(), r3.W(),
            r4.X(), r4.Y(), r4.Z(), r4.W()}
        {}

        BaseMatrix4x4(Span<const float, 16> values) :
            BaseMatrix4x4()
        {
            memcpy(Raw, values.data(), sizeof(ValueType) * 16);
        }

        /// @brief Creates a translation matrix from the given vector3
        /// @param translation The translation value
        /// @return The translation matrix
        static BaseMatrix4x4 CreateTranslation(const BaseVector3<ValueType>& translation)
        {
            BaseMatrix4x4 r = Identity;
            r.M14() = translation.X();
            r.M24() = translation.Y();
            r.M34() = translation.Z();
            return r;
        }

        /// @brief Creates a rotation matrix from the given quaternion
        /// @param rotation The rotation value
        /// @return The rotation matrix
        static BaseMatrix4x4 CreateRotation(const BaseQuaternion<ValueType>& rotation)
        {
            BaseMatrix4x4 mat;

            BaseQuaternion<ValueType> q = rotation.Normalized();

            const ValueType xx = q.X() * q.X();
            const ValueType xy = q.X() * q.Y();
            const ValueType xz = q.X() * q.Z();
            const ValueType xw = q.X() * q.W();

            const ValueType yy = q.Y() * q.Y();
            const ValueType yz = q.Y() * q.Z();
            const ValueType yw = q.Y() * q.W();

            const ValueType zz = q.Z() * q.Z();
            const ValueType zw = q.Z() * q.W();

            // https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
            mat.M11() = static_cast<ValueType>(1.0) - static_cast<ValueType>(2.0) * (yy + zz);
            mat.M12() = static_cast<ValueType>(2.0) * (xy - zw);
            mat.M13() = static_cast<ValueType>(2.0) * (xz + yw);

            mat.M21() = static_cast<ValueType>(2.0) * (xy + zw);
            mat.M22() = static_cast<ValueType>(1.0) - static_cast<ValueType>(2.0) * (xx + zz);
            mat.M23() = static_cast<ValueType>(2.0) * (yz - xw);

            mat.M31() = static_cast<ValueType>(2.0) * (xz - yw);
            mat.M32() = static_cast<ValueType>(2.0) * (yz + xw);
            mat.M33() = static_cast<ValueType>(1.0) - static_cast<ValueType>(2.0) * (xx + yy);

            mat.M44() = static_cast<ValueType>(1.0);

            return mat;
        }

        /// @brief Creates a rotation matrix from the given euler angles. Rotation is applied in Z, Y, X order
        /// @param eulerAngles The euler angles, in radians
        /// @return The rotation matrix
        static BaseMatrix4x4 CreateRotation(const BaseVector3<ValueType>& eulerAngles)
        {
            return CreateRotationX(eulerAngles.X()) * CreateRotationY(eulerAngles.Y()) * CreateRotationZ(eulerAngles.Z());
        }

        /// @brief Creates a rotation matrix along the X axis
        /// @param angleRadians The rotation around the X axis, in radians
        /// @return The rotation matrix
        static BaseMatrix4x4 CreateRotationX(const ValueType& angleRadians)
        {
            BaseMatrix4x4 mat = Identity;

            const ValueType c = Math::Cos(angleRadians);
            const ValueType s = Math::Sin(angleRadians);

            mat.M22() = c;
            mat.M32() = s;

            mat.M23() = -s;
            mat.M33() = c;

            return mat;
        }

        /// @brief Creates a rotation matrix along the Y axis
        /// @param angleRadians The rotation around the Y axis, in radians
        /// @return The rotation matrix
        static BaseMatrix4x4 CreateRotationY(const ValueType& angleRadians)
        {
            BaseMatrix4x4 mat = Identity;

            const ValueType c = Math::Cos(angleRadians);
            const ValueType s = Math::Sin(angleRadians);

            mat.M11() = c;
            mat.M31() = -s;

            mat.M13() = s;
            mat.M33() = c;

            return mat;
        }

        /// @brief Creates a rotation matrix along the Z axis
        /// @param angleRadians The rotation around the Z axis, in radians
        /// @return The rotation matrix
        static BaseMatrix4x4 CreateRotationZ(const ValueType& angleRadians)
        {
            BaseMatrix4x4 mat = Identity;

            const ValueType c = Math::Cos(angleRadians);
            const ValueType s = Math::Sin(angleRadians);

            mat.M11() = c;
            mat.M21() = s;

            mat.M12() = -s;
            mat.M22() = c;

            return mat;
        }

        /// @brief Creates a scale matrix
        /// @param scale The scalar value along each axis
        /// @return The scale matrix
        static BaseMatrix4x4 CreateScale(const BaseVector3<ValueType>& scale)
        {
            BaseMatrix4x4 s;
            s.M11() = scale.X();
            s.M22() = scale.Y();
            s.M33() = scale.Z();
            s.M44() = static_cast<ValueType>(1.0);
            return s;
        }

        /// @brief Creates a view matrix that looks from the eye position to the target position
        /// @param eye The eye position
        /// @param target The target position
        /// @param up The up direction
        /// @return The look at matrix
        static BaseMatrix4x4 CreateLookAt(const BaseVector3<ValueType>& eye, const BaseVector3<ValueType>& target, const BaseVector3<ValueType>& up)
        {
            BaseVector3<ValueType> zAxis = (eye - target).Normalized();
            BaseVector3<ValueType> xAxis = up.Cross(zAxis).Normalized();
            BaseVector3<ValueType> yAxis = zAxis.Cross(xAxis);

            BaseMatrix4x4 v;
            v.M11() = xAxis.X();
            v.M21() = yAxis.X();
            v.M31() = zAxis.X();

            v.M12() = xAxis.Y();
            v.M22() = yAxis.Y();
            v.M32() = zAxis.Y();

            v.M13() = xAxis.Z();
            v.M23() = yAxis.Z();
            v.M33() = zAxis.Z();

            v.M14() = -xAxis.Dot(eye);
            v.M24() = -yAxis.Dot(eye);
            v.M34() = -zAxis.Dot(eye);
            v.M44() = static_cast<ValueType>(1.0);

            return v;
        }

        /// @brief Creates a view matrix
        /// @param position The view position
        /// @param forward The look direction
        /// @param up The up direction
        /// @return The view matrix
        static BaseMatrix4x4 CreateView(const BaseVector3<ValueType>& position, const BaseVector3<ValueType>& forward, const BaseVector3<ValueType>& up)
        {
            BaseVector3<ValueType> right = forward.Cross(up);
            BaseVector3<ValueType> realUp = right.Cross(forward);

            BaseMatrix4x4 v;
            v.M11() = right.X();
            v.M21() = right.Y();
            v.M31() = right.Z();

            v.M12() = realUp.X();
            v.M22() = realUp.Y();
            v.M32() = realUp.Z();

            v.M13() = -forward.X();
            v.M23() = -forward.Y();
            v.M33() = -forward.Z();

            v.M14() = position.X();
            v.M24() = position.Y();
            v.M34() = position.Z();
            v.M44() = static_cast<ValueType>(1.0);

            return v.Inverse();
        }

        /// @brief Creates a view matrix. An identity rotation will result in the view looking in the forward direction
        /// @param position The view position
        /// @param rotation The view rotation
        /// @return The view matrix
        static BaseMatrix4x4 CreateView(const BaseVector3<ValueType>& position, const BaseQuaternion<ValueType>& rotation)
        {
            BaseVector3<ValueType> fwd = rotation * BaseVector3<ValueType>::Forward;
            BaseVector3<ValueType> up = rotation * BaseVector3<ValueType>::Up;
            return CreateView(position, fwd, up);
        }

        /// @brief Creates a transform matrix that represents a position, rotation, and scale
        /// @param position The position
        /// @param rotation The rotation
        /// @param scale The scale
        /// @return The transform matrix
        static BaseMatrix4x4 CreateTransform(const BaseVector3<ValueType>& position, const BaseQuaternion<ValueType>& rotation, const BaseVector3<ValueType>& scale)
        {
            BaseMatrix4x4 t = CreateRotation(rotation) * CreateScale(scale);
            t.M14() = position.X();
            t.M24() = position.Y();
            t.M34() = position.Z();
            return t;
        }

        /// @brief Returns the row 1, column 1 value
        /// @return The row 1, column 1 value
        constexpr ValueType& M11() { return Values[0][0]; }

        /// @brief Returns the row 1, column 1 value
        /// @return The row 1, column 1 value
        constexpr const ValueType& M11() const { return Values[0][0]; }

        /// @brief Returns the row 1, column 2 value
        /// @return The row 1, column 2 value
        constexpr ValueType& M12() { return Values[0][1]; }

        /// @brief Returns the row 1, column 2 value
        /// @return The row 1, column 2 value
        constexpr const ValueType& M12() const { return Values[0][1]; }

        /// @brief Returns the row 1, column 3 value
        /// @return The row 1, column 3 value
        constexpr ValueType& M13() { return Values[0][2]; }

        /// @brief Returns the row 1, column 3 value
        /// @return The row 1, column 3 value
        constexpr const ValueType& M13() const { return Values[0][2]; }

        /// @brief Returns the row 1, column 4 value
        /// @return The row 1, column 4 value
        constexpr ValueType& M14() { return Values[0][3]; }

        /// @brief Returns the row 1, column 4 value
        /// @return The row 1, column 4 value
        constexpr const ValueType& M14() const { return Values[0][3]; }

        /// @brief Returns the row 2, column 1 value
        /// @return The row 2, column 1 value
        constexpr ValueType& M21() { return Values[1][0]; }

        /// @brief Returns the row 2, column 1 value
        /// @return The row 2, column 1 value
        constexpr const ValueType& M21() const { return Values[1][0]; }

        /// @brief Returns the row 2, column 2 value
        /// @return The row 2, column 2 value
        constexpr ValueType& M22() { return Values[1][1]; }

        /// @brief Returns the row 2, column 2 value
        /// @return The row 2, column 2 value
        constexpr const ValueType& M22() const { return Values[1][1]; }

        /// @brief Returns the row 2, column 3 value
        /// @return The row 2, column 3 value
        constexpr ValueType& M23() { return Values[1][2]; }

        /// @brief Returns the row 2, column 3 value
        /// @return The row 2, column 3 value
        constexpr const ValueType& M23() const { return Values[1][2]; }

        /// @brief Returns the row 2, column 4 value
        /// @return The row 2, column 4 value
        constexpr ValueType& M24() { return Values[1][3]; }

        /// @brief Returns the row 2, column 4 value
        /// @return The row 2, column 4 value
        constexpr const ValueType& M24() const { return Values[1][3]; }

        /// @brief Returns the row 3, column 1 value
        /// @return The row 3, column 1 value
        constexpr ValueType& M31() { return Values[2][0]; }

        /// @brief Returns the row 3, column 1 value
        /// @return The row 3, column 1 value
        constexpr const ValueType& M31() const { return Values[2][0]; }

        /// @brief Returns the row 3, column 2 value
        /// @return The row 3, column 2 value
        constexpr ValueType& M32() { return Values[2][1]; }

        /// @brief Returns the row 3, column 2 value
        /// @return The row 3, column 2 value
        constexpr const ValueType& M32() const { return Values[2][1]; }

        /// @brief Returns the row 3, column 3 value
        /// @return The row 3, column 3 value
        constexpr ValueType& M33() { return Values[2][2]; }

        /// @brief Returns the row 3, column 3 value
        /// @return The row 3, column 3 value
        constexpr const ValueType& M33() const { return Values[2][2]; }

        /// @brief Returns the row 3, column 4 value
        /// @return The row 3, column 4 value
        constexpr ValueType& M34() { return Values[2][3]; }

        /// @brief Returns the row 3, column 4 value
        /// @return The row 3, column 4 value
        constexpr const ValueType& M34() const { return Values[2][3]; }

        /// @brief Returns the row 4, column 1 value
        /// @return The row 4, column 1 value
        constexpr ValueType& M41() { return Values[3][0]; }

        /// @brief Returns the row 4, column 1 value
        /// @return The row 4, column 1 value
        constexpr const ValueType& M41() const { return Values[3][0]; }

        /// @brief Returns the row 4, column 2 value
        /// @return The row 4, column 2 value
        constexpr ValueType& M42() { return Values[3][1]; }

        /// @brief Returns the row 4, column 2 value
        /// @return The row 4, column 2 value
        constexpr const ValueType& M42() const { return Values[3][1]; }

        /// @brief Returns the row 4, column 3 value
        /// @return The row 4, column 3 value
        constexpr ValueType& M43() { return Values[3][2]; }

        /// @brief Returns the row 4, column 3 value
        /// @return The row 4, column 3 value
        constexpr const ValueType& M43() const { return Values[3][2]; }

        /// @brief Returns the row 4, column 4 value
        /// @return The row 4, column 4 value
        constexpr ValueType& M44() { return Values[3][3]; }

        /// @brief Returns the row 4, column 4 value
        /// @return The row 4, column 4 value
        constexpr const ValueType& M44() const { return Values[3][3]; }

        /// @brief Returns the nth column of this matrix
        /// @param index The column index [0, 3]
        /// @return The column
        constexpr BaseVector4<ValueType> Column(uint8 index) const
        {
            COCO_ASSERT(index < 4, "Invalid column index");

            return BaseVector4<ValueType>(Values[0][index], Values[1][index], Values[2][index], Values[3][index]);
        }

        /// @brief Gets the X axis vector of this matrix (M11, M21, M31)
        /// @return The X axis direction
        BaseVector3<ValueType> GetXAxis() const { return BaseVector3<ValueType>(M11(), M21(), M31()); }

        /// @brief Gets the Y axis vector of this matrix (M12, M22, M32)
        /// @return The Y axis direction
        BaseVector3<ValueType> GetYAxis() const { return BaseVector3<ValueType>(M12(), M22(), M32()); }

        /// @brief Gets the Z axis vector of this matrix (M13, M23, M33)
        /// @return The Z axis direction
        BaseVector3<ValueType> GetZAxis() const { return BaseVector3<ValueType>(M13(), M23(), M33()); }

        /// @brief Gets the translation value of this matrix (M14, M24, M34)
        /// @return The translation value
        BaseVector3<ValueType> GetTranslation() const {  return BaseVector3<ValueType>(M14(), M24(), M34()); }

        /// @brief Gets the scale value of each axis
        /// @return The scale value
        BaseVector3<ValueType> GetScale() const
        {
            return BaseVector3<ValueType>(GetXAxis().GetLength(), GetYAxis().GetLength(), GetZAxis().GetLength());
        }

        /// @brief Converts the rotation of this matrix into a quaternion
        /// @return The rotation value
        BaseQuaternion<ValueType> GetRotation() const
        {
            ValueType tr = M11() + M22() + M33();

            BaseQuaternion<ValueType> q;

            if (tr > 0)
            {
                ValueType s = Math::Sqrt(tr + static_cast<ValueType>(1.0)) * static_cast<ValueType>(2.0); // S = 4 * qw
                q.W() = static_cast<ValueType>(0.25) * s;
                q.X() = (M32() - M23()) / s;
                q.Y() = (M13() - M31()) / s;
                q.Z() = (M21() - M12()) / s;
            }
            else if (M11() > M22() && M11() > M33())
            {
                ValueType s = Math::Sqrt(static_cast<ValueType>(1.0) + M11() - M22() - M33()) * static_cast<ValueType>(2.0); // S = 4 * qx
                q.W() = (M32() - M23()) / s;
                q.X() = static_cast<ValueType>(0.25) * s;
                q.Y() = (M12() + M21()) / s;
                q.Z() = (M13() + M31()) / s;
            }
            else if (M22() > M33())
            {
                ValueType s = Math::Sqrt(static_cast<ValueType>(1.0) + M22() - M11() - M33()) * static_cast<ValueType>(2.0); // S = 4 * qy
                q.W() = (M13() - M31()) / s;
                q.X() = (M12() + M21()) / s;
                q.Y() = static_cast<ValueType>(0.25) * s;
                q.Z() = (M23() + M32()) / s;
            }
            else
            {
                ValueType s = Math::Sqrt(static_cast<ValueType>(1.0) + M33() - M11() - M22()) * static_cast<ValueType>(2.0); // S = 4 * qz
                q.W() = (M21() - M12()) / s;
                q.X() = (M13() + M31()) / s;
                q.Y() = (M23() + M32()) / s;
                q.Z() = static_cast<ValueType>(0.25) * s;
            }

            return q;
        }

        /// @brief Transposes this matrix, which swaps the columns and the rows
        /// @return The transposed matrix
        BaseMatrix4x4 Transposed() const
        {
            BaseMatrix4x4 r;
            r.M11() = M11();
            r.M12() = M21();
            r.M13() = M31();
            r.M14() = M41();
            r.M21() = M12();
            r.M22() = M22();
            r.M23() = M32();
            r.M24() = M42();
            r.M31() = M13();
            r.M32() = M23();
            r.M33() = M33();
            r.M34() = M43();
            r.M41() = M14();
            r.M42() = M24();
            r.M43() = M34();
            r.M44() = M44();
            return r;
        }

        /// @brief Calculates the inverse of this matrix. When this matrix is multiplied with its inverse, it produces an identity matrix
        /// @return The inverse matrix
        BaseMatrix4x4 Inverse() const
        {
            ValueType t11 = M23() * M34() * M42() - M24() * M33() * M42() + M24() * M32() * M43() - M22() * M34() * M43() - M23() * M32() * M44() + M22() * M33() * M44();
            ValueType t12 = M14() * M33() * M42() - M13() * M34() * M42() - M14() * M32() * M43() + M12() * M34() * M43() + M13() * M32() * M44() - M12() * M33() * M44();
            ValueType t13 = M13() * M24() * M42() - M14() * M23() * M42() + M14() * M22() * M43() - M12() * M24() * M43() - M13() * M22() * M44() + M12() * M23() * M44();
            ValueType t14 = M14() * M23() * M32() - M13() * M24() * M32() - M14() * M22() * M33() + M12() * M24() * M33() + M13() * M22() * M34() - M12() * M23() * M34();

            ValueType det = M11() * t11 + M21() * t12 + M31() * t13 + M41() * t14;
            ValueType idet = static_cast<ValueType>(1.0) / det;

            BaseMatrix4x4 ret;

            ret.M11() = t11 * idet;
            ret.M21() = (M24() * M33() * M41() - M23() * M34() * M41() - M24() * M31() * M43() + M21() * M34() * M43() + M23() * M31() * M44() - M21() * M33() * M44()) * idet;
            ret.M31() = (M22() * M34() * M41() - M24() * M32() * M41() + M24() * M31() * M42() - M21() * M34() * M42() - M22() * M31() * M44() + M21() * M32() * M44()) * idet;
            ret.M41() = (M23() * M32() * M41() - M22() * M33() * M41() - M23() * M31() * M42() + M21() * M33() * M42() + M22() * M31() * M43() - M21() * M32() * M43()) * idet;

            ret.M12() = t12 * idet;
            ret.M22() = (M13() * M34() * M41() - M14() * M33() * M41() + M14() * M31() * M43() - M11() * M34() * M43() - M13() * M31() * M44() + M11() * M33() * M44()) * idet;
            ret.M32() = (M14() * M32() * M41() - M12() * M34() * M41() - M14() * M31() * M42() + M11() * M34() * M42() + M12() * M31() * M44() - M11() * M32() * M44()) * idet;
            ret.M42() = (M12() * M33() * M41() - M13() * M32() * M41() + M13() * M31() * M42() - M11() * M33() * M42() - M12() * M31() * M43() + M11() * M32() * M43()) * idet;

            ret.M13() = t13 * idet;
            ret.M23() = (M14() * M23() * M41() - M13() * M24() * M41() - M14() * M21() * M43() + M11() * M24() * M43() + M13() * M21() * M44() - M11() * M23() * M44()) * idet;
            ret.M33() = (M12() * M24() * M41() - M14() * M22() * M41() + M14() * M21() * M42() - M11() * M24() * M42() - M12() * M21() * M44() + M11() * M22() * M44()) * idet;
            ret.M43() = (M13() * M22() * M41() - M12() * M23() * M41() - M13() * M21() * M42() + M11() * M23() * M42() + M12() * M21() * M43() - M11() * M22() * M43()) * idet;

            ret.M14() = t14 * idet;
            ret.M24() = (M13() * M24() * M31() - M14() * M23() * M31() + M14() * M21() * M33() - M11() * M24() * M33() - M13() * M21() * M34() + M11() * M23() * M34()) * idet;
            ret.M34() = (M14() * M22() * M31() - M12() * M24() * M31() - M14() * M21() * M32() + M11() * M24() * M32() + M12() * M21() * M34() - M11() * M22() * M34()) * idet;
            ret.M44() = (M12() * M23() * M31() - M13() * M22() * M31() + M13() * M21() * M32() - M11() * M23() * M32() - M12() * M21() * M33() + M11() * M22() * M33()) * idet;

            return ret;
        }
    };

    template<typename ValueType>
    BaseVector4<ValueType> operator*(const BaseMatrix4x4<ValueType>& m, const BaseVector4<ValueType>& v)
    {
        return Vector4(m.Row[0].Dot(v), m.Row[1].Dot(v), m.Row[2].Dot(v), m.Row[3].Dot(v));
    }

    template<typename ValueType>
    BaseMatrix4x4<ValueType> operator*(const BaseMatrix4x4<ValueType>& a, const BaseMatrix4x4<ValueType>& b)
    {
        BaseMatrix4x4<ValueType> r;

        for (uint8 c = 0; c < 4; c++)
        {
            r.Row[c].X() = a.Row[c].Dot(b.Column(0));
            r.Row[c].Y() = a.Row[c].Dot(b.Column(1));
            r.Row[c].Z() = a.Row[c].Dot(b.Column(2));
            r.Row[c].W() = a.Row[c].Dot(b.Column(3));
        }

        return r;
    }

    /// @brief A row-major matrix with 4 rows and 4 columns, backed by floating point values
    using Matrix4x4 = BaseMatrix4x4<float>;
}
#endif //COCOENGINE_MATRIX4X4_H