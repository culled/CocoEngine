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
    /*class Math
    {
    public:
        static constexpr float Epsilon = 1e-6;
        static constexpr float EpsilonDouble = 1e-16;
        static constexpr float PI = std::numbers::pi_v<float>;
        static constexpr float TwoPI = PI * 2.0f;

    public:
        static uint64 GetAlignmentOffset(uint64 address, uint64 alignment) noexcept;

        static bool IsZero(int value) noexcept { return value == 0; }

        static bool IsZero(float value) noexcept { return std::abs(value) <= Epsilon; }

        static bool IsZero(double value) noexcept { return std::abs(value) <= EpsilonDouble; }

        static bool IsEqualApprox(float a, float b, float tolerance = Epsilon) noexcept
        {
            return std::abs(a - b) <= tolerance;
        }

        static bool IsEqualApprox(double a, double b, double tolerance = EpsilonDouble) noexcept
        {
            return std::abs(a - b) <= tolerance;
        }

        template<typename ValueType, typename ... Args>
        static ValueType CombineHashes(const ValueType& a, const ValueType& b, Args&& ... args) noexcept
        {
            // https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
            ValueType result = a + 0x9e3779b9 + (b << 6) + (b >> 2);
            return CombineHashes(result, std::forward<Args>(args)...);
        }

        template<typename ValueType>
        static ValueType CombineHashes(const ValueType& a) noexcept
        {
            return a;
        }

        template<typename ValueType>
        static ValueType DegToRad(const ValueType& degrees) noexcept
        {
            return degrees * static_cast<ValueType>(std::numbers::pi_v<ValueType> / 180.0);
        }

        template<typename ValueType>
        static ValueType RadToDeg(const ValueType& radians) noexcept
        {
            return radians * static_cast<ValueType>(180.0 / std::numbers::pi_v<ValueType>);
        }

        template<typename ValueType>
        const ValueType& Max(const ValueType& a, const ValueType& b) noexcept
        {
            return std::max(a, b);
        }

        template<typename ValueType>
        const ValueType& Min(const ValueType& a, const ValueType& b) noexcept
        {
            return std::min(a, b);
        }

        template<typename ValueType>
        ValueType Sin(const ValueType& theta) noexcept
        {
            return std::sin(theta);
        }

        template<typename ValueType>
        ValueType Cos(const ValueType& theta) noexcept
        {
            return std::cos(theta);
        }
    };*/

    constexpr float Epsilon = 1e-6;
    constexpr float EpsilonDouble = 1e-16;
    constexpr float PI = std::numbers::pi_v<float>;
    constexpr float TwoPI = PI * 2.0f;
    constexpr float HalfPI = PI * 0.5f;

    template<typename ValueType>
    const ValueType& Max(const ValueType& a, const ValueType& b) noexcept
    {
        return std::max(a, b);
    }

    template<typename ValueType>
    const ValueType& Min(const ValueType& a, const ValueType& b) noexcept
    {
        return std::min(a, b);
    }

    template<typename ValueType>
    ValueType Sin(const ValueType& theta) noexcept
    {
        return std::sin(theta);
    }

    template<typename ValueType>
    ValueType Cos(const ValueType& theta) noexcept
    {
        return std::cos(theta);
    }

    template<typename ValueType>
    ValueType Tan(const ValueType& theta) noexcept
    {
        return std::tan(theta);
    }

    template<typename ValueType>
    ValueType Asin(const ValueType& theta) noexcept
    {
        return std::asin(theta);
    }

    template<typename ValueType>
    ValueType Acos(const ValueType& theta) noexcept
    {
        return std::acos(theta);
    }

    template<typename ValueType>
    ValueType Atan(const ValueType& theta) noexcept
    {
        return std::atan(theta);
    }

    template<typename ValueType>
    ValueType Atan2(const ValueType& y, const ValueType& x) noexcept
    {
        return std::atan2(y, x);
    }

    template<typename ValueType>
    ValueType Abs(const ValueType& value) noexcept
    {
        return std::abs(value);
    }

    template<typename ValueType>
    ValueType Sqrt(const ValueType& value) noexcept
    {
        return std::sqrt(value);
    }

    template<typename ValueType>
    ValueType Pow(const ValueType& base, const ValueType& exponent) noexcept
    {
        return std::pow(base, exponent);
    }

    template<typename ValueType>
    ValueType Log2(const ValueType& value) noexcept
    {
        return std::log2(value);
    }

    template<typename ValueType>
    ValueType Ceil(const ValueType& value) noexcept
    {
        return std::ceil(value);
    }

    template<typename ValueType>
    ValueType Round(const ValueType& value) noexcept
    {
        return std::round(value);
    }

    template<typename ValueType>
    ValueType Floor(const ValueType& value) noexcept
    {
        return std::floor(value);
    }

    template<typename ValueType>
    ValueType Clamp(const ValueType& value, const ValueType& min, const ValueType& max) noexcept
    {
        return std::clamp(value, min, max);
    }

    template<typename ValueType>
    ValueType Lerp(const ValueType& min, const ValueType& max, const ValueType& factor) noexcept
    {
        return std::lerp(min, max, factor);
    }

    uint64 GetAlignmentOffset(uint64 address, uint64 alignment) noexcept;

    inline bool IsZero(int value) noexcept { return value == 0; }

    inline bool IsZero(float value) noexcept { return Abs(value) <= Epsilon; }

    inline bool IsZero(double value) noexcept { return Abs(value) <= EpsilonDouble; }

    inline bool IsEqualApprox(float a, float b, float tolerance = Epsilon) noexcept
    {
        return Abs(a - b) <= tolerance;
    }

    inline bool IsEqualApprox(double a, double b, double tolerance = EpsilonDouble) noexcept
    {
        return Abs(a - b) <= tolerance;
    }

    template<typename ValueType>
    ValueType CombineHashes(const ValueType& a) noexcept
    {
        return a;
    }

    template<typename ValueType, typename ... Args>
    ValueType CombineHashes(const ValueType& a, const ValueType& b, Args&& ... args) noexcept
    {
        // https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
        ValueType result = a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
        return CombineHashes(result, std::forward<Args>(args)...);
    }

    template<typename ValueType>
    ValueType DegToRad(const ValueType& degrees) noexcept
    {
        return degrees * static_cast<ValueType>(std::numbers::pi_v<ValueType> / 180.0);
    }

    template<typename ValueType>
    ValueType RadToDeg(const ValueType& radians) noexcept
    {
        return radians * static_cast<ValueType>(180.0 / std::numbers::pi_v<ValueType>);
    }
} // Coco::Math

#endif //COCOENGINE_MATH_H