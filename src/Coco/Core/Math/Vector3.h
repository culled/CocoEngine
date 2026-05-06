//
// Created by cullen on 3/7/26.
//

#ifndef COCOENGINE_VECTOR3_H
#define COCOENGINE_VECTOR3_H
#include <type_traits>

#include "Vector2.h"
#include "Math.h"

namespace Coco
{
    template<typename ValueType>
    struct BaseVector3
    {
        static const BaseVector3 Zero;
        static const BaseVector3 Right;
        static const BaseVector3 Left;
        static const BaseVector3 Up;
        static const BaseVector3 Down;
        static const BaseVector3 Backward;
        static const BaseVector3 Forward;
        static const BaseVector3 One;

        ValueType Raw[3];

        BaseVector3() :
            Raw{static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0)}
        {}

        BaseVector3(const ValueType& x, const ValueType& y, const ValueType& z) :
            Raw{x, y, z}
        {}

        BaseVector3(const BaseVector2<ValueType>& xy, const ValueType& z) :
            Raw{xy.X(), xy.Y(), z}
        {}

        BaseVector3 operator+=(const BaseVector3& rhs)
        {
            X() += rhs.X();
            Y() += rhs.Y();
            Z() += rhs.Z();
            return *this;
        }

        BaseVector3 operator-=(const BaseVector3& rhs)
        {
            X() -= rhs.X();
            Y() -= rhs.Y();
            Z() -= rhs.Z();
            return *this;
        }

        BaseVector3 operator*=(const ValueType& scalar)
        {
            X() *= scalar;
            Y() *= scalar;
            Z() *= scalar;
            return *this;
        }

        BaseVector3 operator/=(const ValueType& divisor)
        {
            X() /= divisor;
            Y() /= divisor;
            Z() /= divisor;
            return *this;
        }

        BaseVector3 operator-() const
        {
            return BaseVector3(-X(), -Y(), -Z());
        }

        constexpr ValueType& X() { return Raw[0]; }
        constexpr const ValueType& X() const { return Raw[0]; }

        constexpr ValueType& Y() { return Raw[1]; }
        constexpr const ValueType& Y() const { return Raw[1]; }

        constexpr ValueType& Z() { return Raw[2]; }
        constexpr const ValueType& Z() const { return Raw[2]; }

        ValueType Dot(const BaseVector3& other) const
        {
            return (X() * other.X()) + (Y() * other.Y()) + (Z() * other.Z());
        }

        BaseVector3 Cross(const BaseVector3& other) const
        {
            return BaseVector3(
                (Y() * other.Z()) - (Z() * other.Y()),
                (Z() * other.X()) - (X() * other.Z()),
                (X() * other.Y()) - (Y() * other.X())
            );
        }

        template<typename ReturnType = float>
        ReturnType GetLength() const
        {
            return static_cast<ReturnType>(Math::Sqrt(GetLengthSquared()));
        }

        ValueType GetLengthSquared() const
        {
            return X() * X() + Y() * Y() + Z() * Z();
        }

        void Normalize()
        {
            ValueType length = GetLength();
            if (Math::IsZero(length))
                return;

            ValueType invLength = static_cast<ValueType>(1.0) / length;
            X() *= invLength;
            Y() *= invLength;
            Z() *= invLength;
        }

        BaseVector3 Normalized() const
        {
            BaseVector3 v(*this);
            v.Normalize();
            return v;
        }

        BaseVector3 Orthogonal() const
        {
            ValueType x = Math::Abs(X());
            ValueType y = Math::Abs(Y());
            ValueType z = Math::Abs(Z());

            BaseVector3 other = x < y ? (x < z ? Right : Backward) : (y < z ? Up : Backward);
            return Cross(other);
        }

        BaseVector2<ValueType> XY() const { return BaseVector2<ValueType>(X(), Y()); }
    };

    template<typename ValueType>
    BaseVector3<ValueType> operator+(const BaseVector3<ValueType>& a, const BaseVector3<ValueType>& b)
    {
        return BaseVector3<ValueType>(a.X() + b.X(), a.Y() + b.Y(), a.Z() + b.Z());
    }

    template<typename ValueType>
    BaseVector3<ValueType> operator-(const BaseVector3<ValueType>& a, const BaseVector3<ValueType>& b)
    {
        return BaseVector3<ValueType>(a.X() - b.X(), a.Y() - b.Y(), a.Z() - b.Z());
    }

    template<typename ValueType>
    BaseVector3<ValueType> operator*(const BaseVector3<ValueType>& a, const ValueType& b)
    {
        return BaseVector3<ValueType>(a.X() * b, a.Y() * b, a.Z() * b);
    }

    template<typename ValueType>
    BaseVector3<ValueType> operator/(const BaseVector3<ValueType>& a, const ValueType& b)
    {
        return BaseVector3<ValueType>(a.X() / b, a.Y() / b, a.Z() / b);
    }

    template<typename ValueType, std::enable_if_t<std::is_integral_v<ValueType>>* = nullptr>
    bool operator==(const BaseVector3<ValueType>& a, const BaseVector3<ValueType>& b)
    {
        return a.X() == b.X() && a.Y() == b.Y() && a.Z() == b.Z();
    }

    template<typename ValueType, std::enable_if_t<!std::is_integral_v<ValueType>>* = nullptr>
    bool operator==(const BaseVector3<ValueType>& a, const BaseVector3<ValueType>& b)
    {
        return Math::IsEqualApprox(a.X(), b.X()) &&
            Math::IsEqualApprox(a.Y(), b.Y()) &&
            Math::IsEqualApprox(a.Z(), b.Z());
    }

    template<typename ValueType>
    bool operator!=(const BaseVector3<ValueType>& a, const BaseVector3<ValueType>& b)
    {
        return !(a == b);
    }

    using Vector3 = BaseVector3<float>;
    using Vector3i = BaseVector3<int>;
} // Coco

#endif //COCOENGINE_VECTOR3_H