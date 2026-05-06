//
// Created by cullen on 2/27/26.
//

#ifndef COCOENGINE_VECTOR2_H
#define COCOENGINE_VECTOR2_H
#include "Math.h"
#include <functional>

namespace Coco
{
    template<typename ValueType>
    struct BaseVector2
    {
        static const BaseVector2 Zero;
        static const BaseVector2 Right;
        static const BaseVector2 Left;
        static const BaseVector2 Up;
        static const BaseVector2 Down;
        static const BaseVector2 One;

        ValueType Raw[2];

        BaseVector2() :
            Raw{static_cast<ValueType>(0), static_cast<ValueType>(0)}
        {}

        BaseVector2(const ValueType& x, const ValueType& y) :
            Raw{x, y}
        {}

        BaseVector2 operator+=(const BaseVector2& rhs)
        {
            X() += rhs.X();
            Y() += rhs.Y();
            return *this;
        }

        BaseVector2 operator-=(const BaseVector2& rhs)
        {
            X() -= rhs.X();
            Y() -= rhs.Y();
            return *this;
        }

        BaseVector2 operator*=(const ValueType& scalar)
        {
            X() *= scalar;
            Y() *= scalar;
            return *this;
        }

        BaseVector2 operator/=(const ValueType& divisor)
        {
            X() /= divisor;
            Y() /= divisor;
            return *this;
        }

        BaseVector2 operator-() const
        {
            return BaseVector2(-X(), -Y());
        }

        constexpr ValueType& X() { return Raw[0]; }
        constexpr const ValueType& X() const { return Raw[0]; }

        constexpr ValueType& Y() { return Raw[1]; }
        constexpr const ValueType& Y() const { return Raw[1]; }

        ValueType Dot(const BaseVector2& other) const
        {
            return (X() * other.X()) + (Y() * other.Y());
        }

        template<typename ReturnType = float>
        ReturnType Length() const
        {
            return static_cast<ReturnType>(sqrt(LengthSquared()));
        }

        ValueType LengthSquared() const
        {
            return X() * X() + Y() * Y();
        }

        void Normalize()
        {
            ValueType length = Length();
            if (Math::IsZero(length))
                return;

            ValueType invLength = static_cast<ValueType>(1.0) / length;
            X() *= invLength;
            Y() *= invLength;
        }

        BaseVector2 Normalized() const
        {
            BaseVector2 v(*this);
            v.Normalize();
            return v;
        }
    };

    template<typename ValueType>
    BaseVector2<ValueType> operator+(const BaseVector2<ValueType>& a, const BaseVector2<ValueType>& b)
    {
        return BaseVector2<ValueType>(a.X() + b.X(), a.Y() + b.Y());
    }

    template<typename ValueType>
    BaseVector2<ValueType> operator-(const BaseVector2<ValueType>& a, const BaseVector2<ValueType>& b)
    {
        return BaseVector2<ValueType>(a.X() - b.X(), a.Y() - b.Y());
    }

    template<typename ValueType>
    BaseVector2<ValueType> operator*(const BaseVector2<ValueType>& a, const ValueType& scalar)
    {
        return BaseVector2<ValueType>(a.X() * scalar, a.Y() * scalar);
    }

    template<typename ValueType>
    BaseVector2<ValueType> operator/(const BaseVector2<ValueType>& a, const ValueType& divisor)
    {
        return BaseVector2<ValueType>(a.X() / divisor, a.Y() / divisor);
    }

    template<typename ValueType, std::enable_if_t<std::is_integral_v<ValueType>>* = nullptr>
    bool operator==(const BaseVector2<ValueType>& a, const BaseVector2<ValueType>& b)
    {
        return a.X() == b.X() && a.Y() == b.Y();
    }

    template<typename ValueType, std::enable_if_t<!std::is_integral_v<ValueType>>* = nullptr>
    bool operator==(const BaseVector2<ValueType>& a, const BaseVector2<ValueType>& b)
    {
        return Math::IsEqualApprox(a.X(), b.X()) && Math::IsEqualApprox(a.Y(), b.Y());
    }

    template<typename ValueType>
    bool operator!=(const BaseVector2<ValueType>& a, const BaseVector2<ValueType>& b)
    {
        return !(a == b);
    }

    using Vector2 = BaseVector2<float>;
    using Vector2i = BaseVector2<int>;

    uint64 ToHash(const Vector2i& vector) noexcept;
} // Coco

namespace std
{
    template<>
    struct hash<Coco::Vector2i>
    {
        size_t operator()(const Coco::Vector2i& vector) const noexcept
        {
            return Coco::ToHash(vector);
        }
    };
}
#endif //COCOENGINE_VECTOR2_H