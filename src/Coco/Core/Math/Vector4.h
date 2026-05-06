//
// Created by cullen on 3/12/26.
//

#ifndef COCOENGINE_VECTOR4_H
#define COCOENGINE_VECTOR4_H
#include <type_traits>

#include "Math.h"
#include "Vector2.h"
#include "Vector3.h"

namespace Coco
{
    template<typename ValueType>
    struct BaseVector4
    {
        static const BaseVector4 Zero;
        static const BaseVector4 One;

        ValueType Raw[4];

        BaseVector4() : 
            Raw{static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0), static_cast<ValueType>(0)}
        {}
        
        BaseVector4(const ValueType& x, const ValueType& y, const ValueType& z, const ValueType& w) : 
            Raw{x, y, z, w}
        {}

        BaseVector4(const BaseVector2<ValueType>& xy, const ValueType& z, const ValueType& w) :
            BaseVector4(xy.X(), xy.Y(), z, w) {}
        
        BaseVector4(const BaseVector3<ValueType>& xyz, const ValueType& w) :
            BaseVector4(xyz.X(), xyz.Y(), xyz.Z(), w) {}

        BaseVector4 operator+=(const BaseVector4& rhs)
        {
            X() += rhs.X();
            Y() += rhs.Y();
            Z() += rhs.Z();
            W() += rhs.W();
            return *this;
        }

        BaseVector4 operator-=(const BaseVector4& rhs)
        {
            X() -= rhs.X();
            Y() -= rhs.Y();
            Z() -= rhs.Z();
            W() -= rhs.W();
            return *this;
        }

        BaseVector4 operator*=(const ValueType& scalar)
        {
            X() *= scalar;
            Y() *= scalar;
            Z() *= scalar;
            W() *= scalar;
            return *this;
        }

        BaseVector4 operator/=(const ValueType& divisor)
        {
            X() /= divisor;
            Y() /= divisor;
            Z() /= divisor;
            W() /= divisor;
            return *this;
        }

        BaseVector4 operator-() const
        {
            return BaseVector4(-X(), -Y(), -Z(), -W());
        }

        constexpr ValueType& X() { return Raw[0]; }
        constexpr const ValueType& X() const { return Raw[0]; }

        constexpr ValueType& Y() { return Raw[1]; }
        constexpr const ValueType& Y() const { return Raw[1]; }

        constexpr ValueType& Z() { return Raw[2]; }
        constexpr const ValueType& Z() const { return Raw[2]; }

        constexpr ValueType& W() { return Raw[3]; }
        constexpr const ValueType& W() const { return Raw[3]; }

        ValueType Dot(const BaseVector4& other) const
        {
            return (X() * other.X()) + (Y() * other.Y()) + (Z() * other.Z()) + (W() * other.W());
        }

        template<typename ReturnType = float>
        ReturnType GetLength() const
        {
            return static_cast<ReturnType>(sqrt(GetLengthSquared()));
        }

        ValueType GetLengthSquared() const
        {
            return X() * X() + Y() * Y() + Z() * Z() + W() * W();
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
            W() *= invLength;
        }

        BaseVector4 Normalized() const
        {
            BaseVector4 v(*this);
            v.Normalize();
            return v;
        }

        BaseVector3<ValueType> XYZ() const { return BaseVector3<ValueType>(X(), Y(), Z()); }
        BaseVector2<ValueType> XY() const { return BaseVector2<ValueType>(X(), Y()); }
    };

    template<typename ValueType>
    BaseVector4<ValueType> operator+(const BaseVector4<ValueType>& a, const BaseVector4<ValueType>& b)
    {
        return BaseVector4<ValueType>(
            a.X() + b.X(),
            a.Y() + b.Y(),
            a.Z() + b.Z(),
            a.W() + b.W()
        );
    }

    template<typename ValueType>
    BaseVector4<ValueType> operator-(const BaseVector4<ValueType>& a, const BaseVector4<ValueType>& b)
    {
        return BaseVector4<ValueType>(
            a.X() - b.X(),
            a.Y() - b.Y(),
            a.Z() - b.Z(),
            a.W() - b.W()
        );
    }

    template<typename ValueType>
    BaseVector4<ValueType> operator*(const BaseVector4<ValueType>& a, const ValueType& scalar)
    {
        return BaseVector4<ValueType>(
            a.X() * scalar,
            a.Y() * scalar,
            a.Z() * scalar,
            a.W() * scalar
        );
    }

    template<typename ValueType>
    BaseVector4<ValueType> operator/(const BaseVector4<ValueType>& a, const ValueType& divisor)
    {
        return BaseVector4<ValueType>(
            a.X() / divisor,
            a.Y() / divisor,
            a.Z() / divisor,
            a.W() / divisor
        );
    }

    template<typename ValueType, std::enable_if_t<std::is_integral_v<ValueType>>* = nullptr>
    bool operator==(const BaseVector4<ValueType>& a, const BaseVector4<ValueType>& b)
    {
        return a.X() == b.X() && a.Y() == b.Y()  && a.Z() == b.Z() && a.W() == b.W();
    }

    template<typename ValueType, std::enable_if_t<!std::is_integral_v<ValueType>>* = nullptr>
    bool operator==(const BaseVector4<ValueType>& a, const BaseVector4<ValueType>& b)
    {
        return Math::IsEqualApprox(a.X(), b.X()) && Math::IsEqualApprox(a.Y() , b.Y() ) && Math::IsEqualApprox(a.Z(), b.Z()) && Math::IsEqualApprox(a.W(), b.W());
    }

    template<typename ValueType>
    bool operator!=(const BaseVector4<ValueType>& a, const BaseVector4<ValueType>& b)
    {
        return !(a == b);
    }

    using Vector4 = BaseVector4<float>;
    using Vector4i = BaseVector4<int>;
} // Coco

#endif //COCOENGINE_VECTOR4_H