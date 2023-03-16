#pragma once

#include <Coco/Core/Core.h>

#include <type_traits>
#include <math.h>

namespace Coco
{
	class COCOAPI Math
	{
	public:
		static constexpr double PI = 3.1415926535897932384626433832795;
		static constexpr double DoublePI = PI * 2.0;
		static constexpr double HalfPI = PI * 0.5;
		static constexpr double QuarterPI = PI * 0.25;
		static constexpr double OneOverPI = 1.0 / PI;
		static constexpr double OneOverDoublePI = 1.0 / DoublePI;
		static constexpr double SqrtTwo = 1.4142135623730950488016887242097;
		static constexpr double SqrtThree = 1.7320508075688772935274463415059;
		static constexpr double SqrtOneOverTwo = 0.70710678118654752440084436210485;
		static constexpr double SqrtOneOverThree = 0.57735026918962576450914878050196;
		static constexpr double Deg2RadMultiplier = PI / 180.0;
		static constexpr double Rad2DegMultiplier = 180.0 / PI;

		static constexpr double SecondsToMillisecondsMultiplier = 1000.0;
		static constexpr double MillisecondsToSecondsMultiplier = 1.0 / SecondsToMillisecondsMultiplier;

		static constexpr double Infinity = std::numeric_limits<double>::max();
		static constexpr double Epsilon = std::numeric_limits<double>::epsilon();

	public:
		/// <summary>
		/// Converts degrees to radians
		/// </summary>
		/// <param name="degrees">The degrees</param>
		/// <returns>The equivalent radians</returns>
		static constexpr double Deg2Rad(double degrees) noexcept { return degrees * Deg2RadMultiplier; }

		/// <summary>
		/// Converts radians to degrees
		/// </summary>
		/// <param name="radians">The radians</param>
		/// <returns>The equivalent degrees</returns>
		static constexpr double Rad2Deg(double radians) noexcept { return radians * Rad2DegMultiplier; }

		/// <summary>
		/// Calculates the sine of an angle
		/// </summary>
		/// <param name="x">The angle (in radians)</param>
		/// <returns>The sine</returns>
		static double Sin(const double x) noexcept { return sin(x); }

		/// <summary>
		/// Calculates the cosine of an angle
		/// </summary>
		/// <param name="x">The angle (in radians)</param>
		/// <returns>The cosine</returns>
		static double Cos(const double x) noexcept { return cos(x); }

		/// <summary>
		/// Calculates the tangent of an angle
		/// </summary>
		/// <param name="x">The angle (in radians)</param>
		/// <returns>The tangent</returns>
		static double Tan(const double x) noexcept { return tan(x); }

		/// <summary>
		/// Calculates the arc-sine of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The angle (in radians)</returns>
		static double ASin(const double x) noexcept { return asin(x); }

		/// <summary>
		/// Calculates the arc-cosine of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The angle (in radians)</returns>
		static double ACos(const double x) noexcept { return acos(x); }

		/// <summary>
		/// Calculates the arc-tangent of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The angle (in radians)</returns>
		static double ATan(const double x) noexcept { return atan(x); }

		/// <summary>
		/// Calculates the arc-tangent of an (x,y) vector
		/// </summary>
		/// <param name="x">The x value</param>
		/// <param name="y">The y value</param>
		/// <returns>The angle (in radians) from the x-axis</returns>
		static double ATan2(const double x, const double y) noexcept { return atan2(x, y); }

		/// <summary>
		/// Calculates the square root of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The square root</returns>
		static double Sqrt(const double x) noexcept { return sqrt(x); }

		/// <summary>
		/// Calculates the absolute value of an number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The absolute value of the number</returns>
		template<typename T>
		static T Abs(const T x) noexcept { return abs(x); }

		/// <summary>
		/// Determines if the given value is a power of two
		/// </summary>
		/// <param name="value>The value</param>
		/// <returns>True if the number is a power of two</returns>
		static constexpr bool IsPowerOfTwo(const uint64_t value) noexcept { return (value != 0) && ((value & (value - 1)) == 0); }

		/// <summary>
		/// Compares if two decimal values are within a tolerance of each other.
		/// Use this instead of "a == b" to compare decimals due to rounding errors
		/// </summary>
		/// <param name="a">The first number</param>
		/// <param name="b">The second number</param>
		/// <param name="tolerance">The acceptable error between the two values</param>
		/// <returns>True if the values are within error of each other</returns>
		template<typename T>
		static bool Approximately(const T a, const T b, const T tolerance = Math::Epsilon) noexcept { return Abs(a - b) <= tolerance; }

		/// <summary>
		/// Returns the greater of two numbers
		/// </summary>
		/// <returns>The greater of the two numbers</returns>
		template<typename T>
		static constexpr T Max(const T a, const T b) noexcept { return std::max(a, b); }

		/// <summary>
		/// Returns the smaller of two numbers
		/// </summary>
		/// <returns>The smaller of the two numbers</returns>
		template<typename T>
		static constexpr T Min(T a, T b) noexcept { return std::min(a, b); }

		/// <summary>
		/// Clamps a value between a minimum and a maximum
		/// </summary>
		/// <param name="value">The value</param>
		/// <param name="minimum">The minimum</param>
		/// <param name="maximum">The maximum</param>
		/// <returns>The value clamped in the range [minimum, maximum]</returns>
		template<typename T>
		static constexpr T Clamp(const T value, const T minimum, const T maximum) noexcept { return std::clamp(value, minimum, maximum); }

		/// <summary>
		/// Rounds a number to the closest non-decimal value
		/// </summary>
		/// <param name="value">The value</param>
		/// <returns>The rounded value</returns>
		template<typename T>
		static T Round(const T value) noexcept { return round(value); }

		/// <summary>
		/// Rounds a number to the closest integer
		/// </summary>
		/// <param name="value">The value</param>
		/// <returns>The rounded integer</returns>
		template<typename T>
		static int RoundToInt(const T value) noexcept { return static_cast<int>(Round(value)); }

		/// <summary>
		/// Gets the largest value of a type
		/// </summary>
		/// <returns>The largest value of the type</returns>
		template<typename T>
		static constexpr T MaxValue() noexcept { return std::numeric_limits<T>::max(); }

		/// <summary>
		/// Gets the smallest value of a type
		/// </summary>
		/// <returns>The smallest value of the type</returns>
		template<typename T>
		static constexpr T MinValue() noexcept { return std::numeric_limits<T>::min(); }
	};
}