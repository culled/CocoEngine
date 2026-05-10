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
    /// @brief Base class for a three-dimensional vector
    /// @tparam ValueType The value types
    template<typename ValueType>
    struct BaseVector3
    {
        /// @brief A zero vector (0, 0, 0)
        static const BaseVector3 Zero;

        /// @brief A unit vector pointing to the right (1, 0, 0)
        static const BaseVector3 Right;

        /// @brief A unit vector pointing to the left (-1, 0, 0)
        static const BaseVector3 Left;

        /// @brief A unit vector pointing upwards (0, 1, 0)
        static const BaseVector3 Up;

        /// @brief A unit vector pointing downwards (0, -1, 0)
        static const BaseVector3 Down;

        /// @brief A unit vector pointing backwards (0, 0, 1)
        static const BaseVector3 Backward;

        /// @brief A unit vector pointing forwards (0, 0, -1)
        static const BaseVector3 Forward;

        /// @brief A vector with one in each axis (1, 1, 1)
        static const BaseVector3 One;

        /// @brief The raw vector values, stored in X, Y, Z order
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

        /// @brief The Z Value
        /// @return The Z value
        constexpr ValueType& Z() { return Raw[2]; }

        /// @brief The Z Value
        /// @return The Z value
        constexpr const ValueType& Z() const { return Raw[2]; }

        /// @brief Calculates the dot product of this vector and the given vector. The dot product will be 1 if the vectors point in the same direction, -1 if they point in opposite directions, and 0 if they are perpendicular
        /// @param other The other vector
        /// @return The dot product
        ValueType Dot(const BaseVector3& other) const
        {
            return (X() * other.X()) + (Y() * other.Y()) + (Z() * other.Z());
        }

        /// @brief Computes a vector that is perpendicular to this and the given vector
        /// @param other The other vector
        /// @return A vector perpendicular to this and the other vector
        BaseVector3 Cross(const BaseVector3& other) const
        {
            return BaseVector3(
                (Y() * other.Z()) - (Z() * other.Y()),
                (Z() * other.X()) - (X() * other.Z()),
                (X() * other.Y()) - (Y() * other.X())
            );
        }

        /// @brief Calculates the length of this vector
        /// @tparam ReturnType The return type
        /// @return The length of this vector
        template<typename ReturnType = float>
        ReturnType GetLength() const
        {
            return static_cast<ReturnType>(Math::Sqrt(GetLengthSquared()));
        }

        /// @brief Calculates the squared length of this vector. The normal length calculation involves a square root operation, so this is faster if you don't need the exact length of this vector
        /// @return The squared length of this vector
        ValueType GetLengthSquared() const
        {
            return X() * X() + Y() * Y() + Z() * Z();
        }

        /// @brief Normalizes this vector to unit length
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

        /// @brief Returns the normalized, unit length version of this vector
        /// @return The normalized, unit length vector
        BaseVector3 Normalized() const
        {
            BaseVector3 v(*this);
            v.Normalize();
            return v;
        }

        /// @brief Creates a vector that points perpendicular to this vector
        /// @return The orthogonal vector
        BaseVector3 Orthogonal() const
        {
            ValueType x = Math::Abs(X());
            ValueType y = Math::Abs(Y());
            ValueType z = Math::Abs(Z());

            BaseVector3 other = x < y ? (x < z ? Right : Backward) : (y < z ? Up : Backward);
            return Cross(other);
        }

        /// @brief Creates a two-dimensional vector from this vector's X and Y values
        /// @return The two-dimensional vector
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

    /// @brief A three-dimensional vector backed by floats
    using Vector3 = BaseVector3<float>;

    /// @brief A three-dimensional vector backed by ints
    using Vector3i = BaseVector3<int>;

    /// @brief Computes the hash value of a Vector3i
    /// @param vector The vector
    /// @return The hashed value of the vector
    uint64 ToHash(const Vector3i& vector) noexcept;
} // Coco

namespace std
{
    template<>
    struct hash<Coco::Vector3i>
    {
        size_t operator()(const Coco::Vector3i& vector) const noexcept
        {
            return Coco::ToHash(vector);
        }
    };
}
#endif //COCOENGINE_VECTOR3_H