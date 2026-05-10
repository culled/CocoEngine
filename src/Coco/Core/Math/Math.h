//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_MATH_H
#define COCOENGINE_MATH_H

#include <algorithm>

#include "../Types/CoreTypes.h"
#include <cmath>
#include <numbers>

namespace Coco::Math
{
    /// @brief A small value
    constexpr float Epsilon = 1e-6;

    /// @brief A value smaller than the normal epsilon value which can be used with higher accuracy for double values
    constexpr float EpsilonDouble = 1e-16;

    /// @brief A constant with the value of pi (3.1415...)
    constexpr float PI = std::numbers::pi_v<float>;

    /// @brief Twice the value of pi
    constexpr float TwoPI = PI * 2.0f;

    /// @brief Half the value of pi
    constexpr float HalfPI = PI * 0.5f;

    /// @brief Returns the greater of two values
    /// @tparam ValueType The value type
    /// @param a The first value
    /// @param b The second value
    /// @return The greater of the two values
    template<typename ValueType>
    const ValueType& Max(const ValueType& a, const ValueType& b) noexcept
    {
        return std::max(a, b);
    }

    /// @brief Returns the smaller of two values
    /// @tparam ValueType The value type
    /// @param a The first value
    /// @param b The second value
    /// @return The smaller of the two values
    template<typename ValueType>
    const ValueType& Min(const ValueType& a, const ValueType& b) noexcept
    {
        return std::min(a, b);
    }

    /// @brief Returns the sine of a value
    /// @tparam ValueType The value type
    /// @param x The value, in radians
    /// @return The sine of the value
    template<typename ValueType>
    ValueType Sin(const ValueType& x) noexcept
    {
        return std::sin(x);
    }

    /// @brief Returns the cosine of a value
    /// @tparam ValueType The value type
    /// @param x The value, in radians
    /// @return The cosine of the value
    template<typename ValueType>
    ValueType Cos(const ValueType& x) noexcept
    {
        return std::cos(x);
    }

    /// @brief Returns the tangent of a value
    /// @tparam ValueType The value type
    /// @param x The value, in radians
    /// @return The tangent of the value
    template<typename ValueType>
    ValueType Tan(const ValueType& x) noexcept
    {
        return std::tan(x);
    }

    /// @brief Returns the arcsine of a value
    /// @tparam ValueType The value type
    /// @param x The value
    /// @return The arcsine of the value
    template<typename ValueType>
    ValueType Asin(const ValueType& x) noexcept
    {
        return std::asin(x);
    }

    /// @brief Returns the arccosine of a value
    /// @tparam ValueType The value type
    /// @param x The value
    /// @return The arccosine of the value
    template<typename ValueType>
    ValueType Acos(const ValueType& x) noexcept
    {
        return std::acos(x);
    }

    /// @brief Returns the arctangent of a value
    /// @tparam ValueType The value type
    /// @param x The value
    /// @return The arctangent of the value
    template<typename ValueType>
    ValueType Atan(const ValueType& x) noexcept
    {
        return std::atan(x);
    }

    /// @brief Returns the angle between the positive x-axis and a vector formed from the origin to the point (x, y)
    /// @tparam ValueType The value type
    /// @param y The y value
    /// @param x The x value
    /// @return The angle, in radians, between the positive x-axis and the vector
    template<typename ValueType>
    ValueType Atan2(const ValueType& y, const ValueType& x) noexcept
    {
        return std::atan2(y, x);
    }

    /// @brief Returns the absolute (non-negative) value of a value
    /// @tparam ValueType The value type
    /// @param value The value
    /// @return The absolute value
    template<typename ValueType>
    ValueType Abs(const ValueType& value) noexcept
    {
        return std::abs(value);
    }

    /// @brief Returns the square root of a value
    /// @tparam ValueType The value type
    /// @param value The value
    /// @return The square root of the value
    template<typename ValueType>
    ValueType Sqrt(const ValueType& value) noexcept
    {
        return std::sqrt(value);
    }

    /// @brief Returns a base raised to the power of a given exponent
    /// @tparam ValueType The value type
    /// @param base The value of the base
    /// @param exponent The value of the exponent
    /// @return The base raised to the power of the exponent
    template<typename ValueType>
    ValueType Pow(const ValueType& base, const ValueType& exponent) noexcept
    {
        return std::pow(base, exponent);
    }

    /// @brief Returns the base-2 logarithm of a value
    /// @tparam ValueType The value type
    /// @param value The value
    /// @return The base-2 logarithm of a value
    template<typename ValueType>
    ValueType Log2(const ValueType& value) noexcept
    {
        return std::log2(value);
    }

    /// @brief Returns the base-10 logarithm of a value
    /// @tparam ValueType The value type
    /// @param value The value
    /// @return The base-10 logarithm of a value
    template<typename ValueType>
    ValueType Log10(const ValueType& value) noexcept
    {
        return std::log10(value);
    }

    /// @brief Returns the value rounded up to the next-highest integral value
    /// @tparam ValueType The value type
    /// @param value The value
    /// @return The value rounded up to the next highest integral value
    template<typename ValueType>
    ValueType Ceil(const ValueType& value) noexcept
    {
        return std::ceil(value);
    }

    /// @brief Returns the value rounded to the nearest integral value
    /// @tparam ValueType The value type
    /// @param value The value
    /// @return The value rounded to the nearest integral value
    template<typename ValueType>
    ValueType Round(const ValueType& value) noexcept
    {
        return std::round(value);
    }

    /// @brief Returns the value rounded down to the next-lowest integral value
    /// @tparam ValueType The value type
    /// @param value The value
    /// @return The value rounded down to the next-lowest integral value
    template<typename ValueType>
    ValueType Floor(const ValueType& value) noexcept
    {
        return std::floor(value);
    }

    /// @brief Returns the value clamped to the range [min, max]
    /// @tparam ValueType The value type
    /// @param value The value
    /// @param min The minimum value
    /// @param max The maximum value
    /// @return The clamped value
    template<typename ValueType>
    ValueType Clamp(const ValueType& value, const ValueType& min, const ValueType& max) noexcept
    {
        return std::clamp(value, min, max);
    }

    /// @brief Linearly interpolates between a minimum and maximum value
    /// @tparam ValueType The value type
    /// @param min The minimum value
    /// @param max The maximum value
    /// @param factor The interpolation factor. 0 results in the minimum value, 1 results in the maximum value, 0.5 is halfway between the two
    /// @return The linearly interpolated value
    template<typename ValueType>
    ValueType Lerp(const ValueType& min, const ValueType& max, const ValueType& factor) noexcept
    {
        return std::lerp(min, max, factor);
    }

    /// @brief Returns an address that is a multiple of the given alignment
    /// @param address The memory address
    /// @param alignment The desired alignment
    /// @return The address aligned to the next higher multiple of the alignment. It will always be equal to or greater than the original address
    uint64 AlignedAddress(uint64 address, uint64 alignment) noexcept;

    /// @brief Determines if the given value is equal to zero
    /// @param value The value
    /// @return True if the value is equal to zero
    inline bool IsZero(int value) noexcept { return value == 0; }

    /// @brief Determines if the given value is equal to zero
    /// @param value The value
    /// @return True if the value is equal to zero
    inline bool IsZero(float value) noexcept { return Abs(value) <= Epsilon; }

    /// @brief Determines if the given value is equal to zero
    /// @param value The value
    /// @return True if the value is equal to zero
    inline bool IsZero(double value) noexcept { return Abs(value) <= EpsilonDouble; }

    /// @brief Determines if two values are within range of each other
    /// @param a The first value
    /// @param b The second value
    /// @param tolerance The maximum difference between the values to consider them equal
    /// @return True if the difference between the two values is within the given tolerance
    inline bool IsEqualApprox(float a, float b, float tolerance = Epsilon) noexcept
    {
        return Abs(a - b) <= tolerance;
    }

    /// @brief Determines if two values are within range of each other
    /// @param a The first value
    /// @param b The second value
    /// @param tolerance The maximum difference between the values to consider them equal
    /// @return True if the difference between the two values is within the given tolerance
    inline bool IsEqualApprox(double a, double b, double tolerance = EpsilonDouble) noexcept
    {
        return Abs(a - b) <= tolerance;
    }

    /// @brief Closes the CombineHashes() template chain
    /// @tparam ValueType The type of value
    /// @param a The value
    /// @return The combined hash
    template<typename ValueType>
    ValueType CombineHashes(const ValueType& a) noexcept
    {
        return a;
    }

    /// @brief Combines two values into a single hash value
    /// @tparam ValueType The type of value
    /// @tparam Args The type of remaining values
    /// @param a The first value
    /// @param b The second value
    /// @param args The remaining values
    /// @return The combined hash value
    template<typename ValueType, typename ... Args>
    ValueType CombineHashes(const ValueType& a, const ValueType& b, Args&& ... args) noexcept
    {
        // https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
        ValueType result = a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
        return CombineHashes(result, std::forward<Args>(args)...);
    }

    /// @brief Converts an angle from degrees into radians
    /// @tparam ValueType The value type
    /// @param degrees The angle, in degrees
    /// @return The equivalent angle in radians
    template<typename ValueType>
    ValueType DegToRad(const ValueType& degrees) noexcept
    {
        return degrees * static_cast<ValueType>(std::numbers::pi_v<ValueType> / 180.0);
    }

    /// @brief Converts an angle from radians into degrees
    /// @tparam ValueType The value type
    /// @param radians The angle, in radians
    /// @return The equivalent angle in degrees
    template<typename ValueType>
    ValueType RadToDeg(const ValueType& radians) noexcept
    {
        return radians * static_cast<ValueType>(180.0 / std::numbers::pi_v<ValueType>);
    }
} // Coco::Math

#endif //COCOENGINE_MATH_H