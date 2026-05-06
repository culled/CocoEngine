//
// Created by cullen on 3/11/26.
//

#ifndef COCOENGINE_QUATERNION_H
#define COCOENGINE_QUATERNION_H
#include "Vector3.h"

namespace Coco
{
    template<typename ValueType>
    struct BaseQuaternion
    {
        static const BaseQuaternion Identity;

        ValueType Raw[4];

        BaseQuaternion() :
            Raw{static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(1.0)}
        {}

        BaseQuaternion(ValueType x, ValueType y, ValueType z, ValueType w) :
            Raw{x, y, z, w}
        {}

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

        constexpr ValueType& X() { return Raw[0]; }
        constexpr const ValueType& X() const { return Raw[0]; }

        constexpr ValueType& Y() { return Raw[1]; }
        constexpr const ValueType& Y() const { return Raw[1]; }

        constexpr ValueType& Z() { return Raw[2]; }
        constexpr const ValueType& Z() const { return Raw[2]; }

        constexpr ValueType& W() { return Raw[3]; }
        constexpr const ValueType& W() const { return Raw[3]; }

        void Invert()
        {
            X() = -X();
            Y() = -Y();
            Z() = -Z();
        }

        BaseQuaternion Inverted() const
        {
            BaseQuaternion q(*this);
            q.Invert();
            return q;
        }

        ValueType GetNormal() const
        {
            return Math::Sqrt(X() * X() + Y() * Y() + Z() * Z() + W() * W());
        }

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

        BaseQuaternion Normalized() const
        {
            BaseQuaternion q(*this);
            q.Normalize();
            return q;
        }

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
                //eulerAngles.Z() = 0.0;
            }
            else if (test < -unit * unitTest)
            {
                // Singularity at south pole
                eulerAngles.Y() = Math::Atan2(static_cast<ValueType>(2.0) * (Y() * W() - X() * Z()), xx - yy - zz + ww);
                eulerAngles.X() = -Math::HalfPI;
                //eulerAngles.Z() = 0.0;
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

    using Quaternion = BaseQuaternion<float>;
} // Coco

#endif //COCOENGINE_QUATERNION_H