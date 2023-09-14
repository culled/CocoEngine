#pragma once

#include <Coco/Core/Corepch.h>
#include "../Defines.h"

namespace Coco::Math
{
	/// @brief Gets the maximum of two values
	/// @tparam ValueType The type of values
	/// @param a The first value
	/// @param b The second value
	/// @return The greater of the two values
	template<typename ValueType>
	constexpr const ValueType& Max(const ValueType& a, const ValueType& b) { return std::max<ValueType>(a, b); }

	/// @brief Gets the minimum of two values
	/// @tparam ValueType The type of values
	/// @param a The first value
	/// @param b The second value
	/// @return The lesser of the two values
	template<typename ValueType>
	constexpr const ValueType& Min(const ValueType& a, const ValueType& b) { return std::min<ValueType>(a, b); }

	/// @brief Clamps a value between a minimum and a maximum
	/// @tparam ValueType The type of values
	/// @param v The value
	/// @param min The minimum value
	/// @param max The maximum value
	/// @return The clamped value
	template<typename ValueType>
	constexpr const ValueType& Clamp(const ValueType& v, const ValueType& min, const ValueType& max) { return std::clamp<ValueType>(v, min, max); }

	/// @brief Rounds a value to the nearest whole number
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The rounded value
	template<typename ValueType>
	constexpr ValueType Round(const ValueType& v) { return std::round(v); }

	/// @brief Gets the maximum value that a type can hold
	/// @tparam ValueType The type of value
	template<typename ValueType>
	constexpr auto MaxValue = std::numeric_limits<ValueType>::max;

	/// @brief Gets the smallest discernable value of a type
	/// @tparam ValueType The type of value
	template<typename ValueType>
	constexpr auto EpsilonValue = std::numeric_limits<ValueType>::epsilon;

	/// @brief The smallest discernable value of a double 
	constexpr double Epsilon = EpsilonValue<double>();

	/// @brief The smallest discernable value of a float 
	constexpr float EpsilonF = EpsilonValue<float>();

	/// @brief Gets the absolute value of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The absolute value
	template<typename ValueType>
	constexpr ValueType Abs(const ValueType& v) { return std::abs(v); }

	/// @brief Raises the given base by the exponent
	/// @tparam ValueType The type of value
	/// @param base The base
	/// @param exp The exponent
	/// @return The base raised to the power of the exponent
	template<typename ValueType>
	constexpr ValueType Pow(const ValueType& base, const ValueType& exp) { return std::pow(base, exp); }

	/// @brief Gets the square root of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The square root
	template<typename ValueType>
	constexpr double Sqrt(const ValueType& v) { return std::sqrt(v); }

	/// @brief Tests if two decimal values are within range of each other
	/// @tparam ValueType The type of value
	/// @param a The first number
	/// @param b The second number
	/// @param threshold The threshold that the two numbers must be within range of each other.
	/// @return True if the two numbers are within the threshold of each other
	template<typename ValueType>
	constexpr bool Approximately(const ValueType& a, const ValueType& b, const ValueType& threshold)
	{
		return Abs(a - b) <= threshold * Max(Abs(a), Abs(b));
	}

	/// @brief Tests two decimal types for equality
	/// @tparam ValueType The type
	/// @param a The first number
	/// @param b The second number
	/// @return True if the two are equal within an acceptable error of each other
	template<typename ValueType>
	constexpr bool Equal(const ValueType& a, const ValueType& b) { return Approximately(a, b, EpsilonValue<ValueType>()); }
}