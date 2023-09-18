#pragma once

#include <Coco/Core/Corepch.h>
#include "../Defines.h"

namespace Coco::Math
{
	constexpr double PI = std::numbers::pi;
	constexpr double HalfPI = PI / 2.0;
	constexpr double InvPI = std::numbers::inv_pi;
	constexpr double Sqrt2 = std::numbers::sqrt2;
	constexpr double Deg2RadMultiplier = PI / 180.0;
	constexpr double Rad2DegMultiplier = 180.0 / PI;

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
	ValueType Abs(const ValueType& v) { return std::abs(v); }

	/// @brief Raises the given base by the exponent
	/// @tparam ValueType The type of value
	/// @param base The base
	/// @param exp The exponent
	/// @return The base raised to the power of the exponent
	template<typename ValueType>
	ValueType Pow(const ValueType& base, const ValueType& exp) { return std::pow(base, exp); }

	/// @brief Gets the square root of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The square root
	template<typename ValueType>
	double Sqrt(const ValueType& v) { return std::sqrt(v); }

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

	/// @brief Combines hashes using a seed
	/// @param seed The seed to use when combining
	/// @return The combined hash
	constexpr uint64 CombineHashes(uint64 seed) { return seed; }

	/// @brief Combines hashes using a seed
	/// @tparam ...Hashes The types of hashes
	/// @param seed The seed to use when combining
	/// @param value The first hash
	/// @param ...hashes The remaining hashes
	/// @return The combined hash
	template<typename ... Hashes>
	constexpr uint64 CombineHashes(uint64 seed, uint64 value, Hashes... hashes)
	{
		// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
		std::hash<uint64> hasher;
		seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return CombineHashes(seed, hashes...);
	}

	/// @brief Calculates the cosine of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The cosine of the value
	template<typename ValueType>
	ValueType Cos(const ValueType& v) { return std::cos(v); }

	/// @brief Calculates the arc-cosine of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The arc-cosine of the value
	template<typename ValueType>
	ValueType Acos(const ValueType& v) { return std::acos(v); }

	/// @brief Calculates the sine of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The sine of the value
	template<typename ValueType>
	ValueType Sin(const ValueType& v) { return std::sin(v); }

	/// @brief Calculates the arc-sine of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The arc-sine of the value
	template<typename ValueType>
	ValueType Asin(const ValueType& v) { return std::asin(v); }

	/// @brief Calculates the tangent of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The tangent of the value
	template<typename ValueType>
	ValueType Tan(const ValueType& v) { return std::tan(v); }

	/// @brief Calculates the arc-tangent of a value
	/// @tparam ValueType The type of value
	/// @param v The value
	/// @return The arc-tangent of the value
	template<typename ValueType>
	ValueType Atan(const ValueType& v) { return std::atan(v); }

	/// @brief Calcualates the atan2 value of an (x, y) coordinate
	/// @tparam ValueType The type of value
	/// @param y The Y value
	/// @param x The X value
	/// @return The angle
	template<typename ValueType>
	ValueType Atan2(const ValueType& y, const ValueType& x) { return std::atan2(y, x); }

	/// @brief Converts degrees to radians
	/// @tparam ValueType The value type
	/// @param deg The degrees
	/// @return The converted radians
	template<typename ValueType>
	constexpr ValueType DegToRad(const ValueType& deg) { return deg * static_cast<ValueType>(Deg2RadMultiplier); }

	/// @brief Converts radians to degrees
	/// @tparam ValueType The value type
	/// @param rad The radians
	/// @return The converted degrees
	template<typename ValueType>
	constexpr ValueType RadToDeg(const ValueType& rad) { return rad * static_cast<ValueType>(Rad2DegMultiplier); }
}