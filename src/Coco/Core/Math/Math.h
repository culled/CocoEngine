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
	constexpr auto Epsilon = std::numeric_limits<ValueType>::epsilon;

	/// @brief Gets the absolute value of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The absolute value
	template<typename ValueType>
	constexpr ValueType Abs(const ValueType& v) { return std::abs(v); }

	/// @brief Tests two decimal types for equality
	/// @tparam ValueType The type
	/// @param a The first number
	/// @param b The second number
	/// @return True if the two are equal within an acceptable error of each other
	template<typename ValueType>
	constexpr bool Equal(const ValueType& a, const ValueType& b)
	{
		return Abs(a - b) <= Epsilon<ValueType>() * Max(Abs(a), Abs(b));
	}
}