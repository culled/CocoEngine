//
// Created by cullen on 2/27/26.
//

#ifndef COCOENGINE_VECTOR2_H
#define COCOENGINE_VECTOR2_H
#include "Math.h"
#include <functional>

namespace Coco
{
    /// @brief Base class for a two-dimensional vector
    /// @tparam ValueType The value types
    template<typename ValueType>
    struct BaseVector2
    {
        /// @brief A zero vector (0, 0)
        static const BaseVector2 Zero;

        /// @brief A unit vector pointing to the right (1, 0)
        static const BaseVector2 Right;

        /// @brief A unit vector pointing to the left (-1, 0)
        static const BaseVector2 Left;

        /// @brief A unit vector pointing upwards (0, 1)
        static const BaseVector2 Up;

        /// @brief A unit vector pointing downwards (0, -1)
        static const BaseVector2 Down;

        /// @brief A vector with one in each axis (1, 1)
        static const BaseVector2 One;

        /// @brief The raw vector values, stored in X, Y order
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

        /// @brief The X Value
        /// @return The X value
        constexpr ValueType& X() { return Raw[0]; }

        /// @brief The X Value
        /// @return The X value
        constexpr const ValueType& X() const { return Raw[0]; }

        /// @brief The Y Value
        /// @return The Y value
        constexpr ValueType& Y() { return Raw[1]; }

        /// @brief The Y Value
        /// @return The Y value
        constexpr const ValueType& Y() const { return Raw[1]; }

        /// @brief Calculates the dot product of this vector and the given vector. The dot product will be 1 if the vectors point in the same direction, -1 if they point in opposite directions, and 0 if they are perpendicular
        /// @param other The other vector
        /// @return The dot product
        ValueType Dot(const BaseVector2& other) const
        {
            return (X() * other.X()) + (Y() * other.Y());
        }

        /// @brief Calculates the length of this vector
        /// @tparam ReturnType The return type
        /// @return The length of this vector
        template<typename ReturnType = float>
        ReturnType Length() const
        {
            return static_cast<ReturnType>(sqrt(LengthSquared()));
        }

        /// @brief Calculates the squared length of this vector. The normal length calculation involves a square root operation, so this is faster if you don't need the exact length of this vector
        /// @return The squared length of this vector
        ValueType LengthSquared() const
        {
            return X() * X() + Y() * Y();
        }

        /// @brief Normalizes this vector to unit length
        void Normalize()
        {
            ValueType length = Length();
            if (Math::IsZero(length))
                return;

            ValueType invLength = static_cast<ValueType>(1.0) / length;
            X() *= invLength;
            Y() *= invLength;
        }

        /// @brief Returns the normalized, unit length version of this vector
        /// @return The normalized, unit length vector
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

    /// @brief A two-dimensional vector backed by floats
    using Vector2 = BaseVector2<float>;

    /// @brief A two-dimensional vector backed by ints
    using Vector2i = BaseVector2<int>;

    /// @brief Computes the hash value of a Vector2i
    /// @param vector The vector
    /// @return The hashed value of the vector
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