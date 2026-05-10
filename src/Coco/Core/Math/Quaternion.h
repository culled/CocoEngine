//
// Created by cullen on 3/11/26.
//

#ifndef COCOENGINE_QUATERNION_H
#define COCOENGINE_QUATERNION_H
#include "Vector3.h"

namespace Coco
{
    /// @brief Base class for a quaternion rotation
    /// @tparam ValueType The value type
    template<typename ValueType>
    struct BaseQuaternion
    {
        /// @brief An identity quaternion, representing no rotation
        static const BaseQuaternion Identity;

        /// @brief The raw values, stored in X, Y, Z, W order
        ValueType Raw[4];

        BaseQuaternion() :
            Raw{static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(1.0)}
        {}

        BaseQuaternion(ValueType x, ValueType y, ValueType z, ValueType w) :
            Raw{x, y, z, w}
        {}

        BaseQuaternion(const BaseVector3<ValueType>& axis, ValueType angleRadians)
        {
            const ValueType halfAngle = angleRadians * static_cast<ValueType>(0.5);
            const ValueType s = Math::Sin(halfAngle);
            const ValueType c = Math::Cos(halfAngle);
            const BaseVector3<ValueType> normalizedAxis = axis.Normalized();

            X() = s * normalizedAxis.X();
            Y() = s * normalizedAxis.Y();
            Z() = s * normalizedAxis.Z();
            W() = c;
        }

        BaseQuaternion(const BaseVector3<ValueType>& eulerAngles) :
            BaseQuaternion()
        {
            BaseVector3<ValueType> halfAngles = eulerAngles * static_cast<ValueType>(0.5);

            // Pitch (X), Roll (Z), Yaw (Y) - YXZ order
            ValueType c1 = Math::Cos(halfAngles.Y());
            ValueType s1 = Math::Sin(halfAngles.Y());
            ValueType c2 = Math::Cos(halfAngles.X());
            ValueType s2 = Math::Sin(halfAngles.X());
            ValueType c3 = Math::Cos(halfAngles.Z());
            ValueType s3 = Math::Sin(halfAngles.Z());

            W() = c1 * c2 * c3 - (-s1 * s2) * s3;
            X() = c1 * s2 * c3 + s1 * c2 * s3;
            Y() = -(c1 * s2) * s3 + s1 * c2 * c3;
            Z() = -(s1 * s2) * c3 + c1 * c2 * s3;
        }

        /// @brief Computes the shortest rotation to rotate "startDir" to "endDir"
        /// @param startDir The starting direction
        /// @param endDir The ending direction
        /// @return The shortest rotation to rotate "startDir" to "endDir"
        static BaseQuaternion FromToRotation(const BaseVector3<ValueType>& startDir, const BaseVector3<ValueType>& endDir)
        {
            ValueType d = startDir.Dot(endDir);
            ValueType k = Math::Sqrt(startDir.GetLengthSquared() * endDir.GetLengthSquared());

            if (d / k <= static_cast<ValueType>(-1.0 + Math::Epsilon))
            {
                BaseVector3<ValueType> o = startDir.Orthogonal();
                return BaseQuaternion(o.X(), o.Y(), o.Z(), static_cast<ValueType>(0.0));
            }

            BaseVector3<ValueType> c = startDir.Cross(endDir);
            BaseQuaternion q(c.X(), c.Y(), c.Z(), d + k);
            q.Normalize();
            return q;
        }

        /// @brief The X value
        /// @return The X Value
        constexpr ValueType& X() { return Raw[0]; }

        /// @brief The X value
        /// @return The X Value
        constexpr const ValueType& X() const { return Raw[0]; }

        /// @brief The Y value
        /// @return The Y Value
        constexpr ValueType& Y() { return Raw[1]; }

        /// @brief The Y value
        /// @return The Y Value
        constexpr const ValueType& Y() const { return Raw[1]; }

        /// @brief The Z value
        /// @return The Z Value
        constexpr ValueType& Z() { return Raw[2]; }

        /// @brief The Z value
        /// @return The Z Value
        constexpr const ValueType& Z() const { return Raw[2]; }

        /// @brief The W value
        /// @return The W Value
        constexpr ValueType& W() { return Raw[3]; }

        /// @brief The W value
        /// @return The W Value
        constexpr const ValueType& W() const { return Raw[3]; }

        /// @brief Inverts this rotation
        void Invert()
        {
            X() = -X();
            Y() = -Y();
            Z() = -Z();
        }

        /// @brief Gets the inverse of this rotation. Multiplying this rotation by its inverse produces an identity rotation
        /// @return The inverted rotation
        BaseQuaternion Inverted() const
        {
            BaseQuaternion q(*this);
            q.Invert();
            return q;
        }

        /// @brief Gets the normal, or norm, of this quaternion
        /// @return The normal
        ValueType GetNormal() const
        {
            return Math::Sqrt(X() * X() + Y() * Y() + Z() * Z() + W() * W());
        }

        /// @brief Normalizes this rotation, ensuring it is unit length
        void Normalize()
        {
            ValueType length = GetNormal();
            if (Math::IsZero(length))
                return;

            ValueType invLength = static_cast<ValueType>(1.0) / length;
            X() *= invLength;
            Y() *= invLength;
            Z() *= invLength;
            W() *= invLength;
        }

        /// @brief Returns the normalized, unit length version of this rotation
        /// @return The normalized rotation
        BaseQuaternion Normalized() const
        {
            BaseQuaternion q(*this);
            q.Normalize();
            return q;
        }

        /// @brief Converts this rotation into euler angles, with rotation applied in the Z, Y, X order
        /// @return The euler angles, in radians
        BaseVector3<ValueType> GetEulerAngles() const
        {
            // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

            ValueType ww = W() * W();
            ValueType xx = X() * X();
            ValueType yy = Y() * Y();
            ValueType zz = Z() * Z();
            ValueType unit = xx + yy + zz + ww; // If normalised is one, otherwise is correction factor
            ValueType test = X() * W() - Y() * Z();

            BaseVector3<ValueType> eulerAngles;

            const ValueType unitTest = static_cast<ValueType>(0.5 - Math::Epsilon);
            if (test > unit * unitTest)
            {
                // Singularity at north pole
                //eulerAngles.Y = Math::Atan2(2.0 * (Y * W - X * Z), 1.0 - 2.0 * (yy + zz));
                eulerAngles.Y() = Math::Atan2(static_cast<ValueType>(2.0) * (Y() * W() - X() * Z()), xx - yy - zz + ww);
                eulerAngles.X() = Math::HalfPI;
            }
            else if (test < -unit * unitTest)
            {
                // Singularity at south pole
                eulerAngles.Y() = Math::Atan2(static_cast<ValueType>(2.0) * (Y() * W() - X() * Z()), xx - yy - zz + ww);
                eulerAngles.X() = -Math::HalfPI;
            }
            else
            {
                // No singularity
                eulerAngles.Y() = Math::Atan2(static_cast<ValueType>(2.0) * (X() * Z() + Y() * W()), ww - xx - yy + zz);
                eulerAngles.X() = Math::Asin(static_cast<ValueType>(-2.0) * (Y() * Z() - W() * X()));
                eulerAngles.Z() = Math::Atan2(static_cast<ValueType>(2.0) * (X() * Y() + W() * Z()), ww - xx + yy - zz);
            }

            return eulerAngles;
        }
    };

    template<typename ValueType>
    BaseVector3<ValueType> operator*(const BaseQuaternion<ValueType>& quat, const BaseVector3<ValueType>& vector)
    {
        // https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
        BaseVector3<ValueType> u(quat.X(), quat.Y(), quat.Z());

        return u * static_cast<ValueType>(2.0) * u.Dot(vector) +
            vector * (quat.W() *  quat.W() - u.GetLengthSquared()) +
            u.Cross(vector) * static_cast<ValueType>(2.0) * quat.W();
    }

    /// @brief A quaternion rotation backed by floating point numbers
    using Quaternion = BaseQuaternion<float>;
} // Coco

#endif //COCOENGINE_QUATERNION_H