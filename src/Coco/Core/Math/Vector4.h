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
    /// @brief Base class for a four-dimensional vector
    /// @tparam ValueType The value types
    template<typename ValueType>
    struct BaseVector4
    {
        /// @brief A zero vector (0, 0, 0, 0)
        static const BaseVector4 Zero;

        /// @brief A vector with one in each axis (1, 1, 1, 1)
        static const BaseVector4 One;

        /// @brief The raw vector values, stored in X, Y, Z, W order
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

        /// @brief The W Value
        /// @return The W value
        constexpr ValueType& W() { return Raw[3]; }

        /// @brief The W Value
        /// @return The W value
        constexpr const ValueType& W() const { return Raw[3]; }

        /// @brief Calculates the dot product of this vector and the given vector. The dot product will be 1 if the vectors point in the same direction, -1 if they point in opposite directions, and 0 if they are perpendicular
        /// @param other The other vector
        /// @return The dot product
        ValueType Dot(const BaseVector4& other) const
        {
            return (X() * other.X()) + (Y() * other.Y()) + (Z() * other.Z()) + (W() * other.W());
        }

        /// @brief Calculates the length of this vector
        /// @tparam ReturnType The return type
        /// @return The length of this vector
        template<typename ReturnType = float>
        ReturnType GetLength() const
        {
            return static_cast<ReturnType>(sqrt(GetLengthSquared()));
        }

        /// @brief Calculates the squared length of this vector. The normal length calculation involves a square root operation, so this is faster if you don't need the exact length of this vector
        /// @return The squared length of this vector
        ValueType GetLengthSquared() const
        {
            return X() * X() + Y() * Y() + Z() * Z() + W() * W();
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
            W() *= invLength;
        }

        /// @brief Returns the normalized, unit length version of this vector
        /// @return The normalized, unit length vector
        BaseVector4 Normalized() const
        {
            BaseVector4 v(*this);
            v.Normalize();
            return v;
        }

        /// @brief Creates a three-dimensional vector from this vector's X, Y, and Z values
        /// @return The three-dimensional vector
        BaseVector3<ValueType> XYZ() const { return BaseVector3<ValueType>(X(), Y(), Z()); }

        /// @brief Creates a two-dimensional vector from this vector's X and Y values
        /// @return The two-dimensional vector
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

    /// @brief A four-dimensional vector backed by floats
    using Vector4 = BaseVector4<float>;

    /// @brief A four-dimensional vector backed by ints
    using Vector4i = BaseVector4<int>;
} // Coco

#endif //COCOENGINE_VECTOR4_H