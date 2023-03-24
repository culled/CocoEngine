#pragma once

#include <Coco/Core/Core.h>

#include <type_traits>
#include <math.h>

namespace Coco
{
	/// @brief A math function library
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
		/// @brief Converts degrees to radians
		/// @param degrees The degrees
		/// @return The equivalent angle in radians
		static constexpr double Deg2Rad(double degrees) noexcept { return degrees * Deg2RadMultiplier; }

		/// @brief Converts radians to degrees
		/// @param radians The radians
		/// @return The equivalent angle in degrees
		static constexpr double Rad2Deg(double radians) noexcept { return radians * Rad2DegMultiplier; }

		/// @brief Calculates the sine of an angle
		/// @param x The angle (in radians)
		/// @return The sine value
		static double Sin(double x) noexcept { return sin(x); }

		/// @brief Calculates the cosine of an angle
		/// @param x The angle (in radians)
		/// @return The cosine value
		static double Cos(double x) noexcept { return cos(x); }

		/// @brief Calculates the tangent of an angle
		/// @param x The angle (in radians)
		/// @return The tangent value
		static double Tan(double x) noexcept { return tan(x); }

		/// @brief Calculates the arc-sine of a number
		/// @param x The ratio between the opposite and adjacent side lengths
		/// @return The angle (in radians)
		static double Asin(double x) noexcept { return asin(x); }

		/// @brief Calculates the arc-cosine of a number
		/// @param x The ratio between the adjacent and hypotenuse side lengths
		/// @return The angle (in radians)
		static double Acos(double x) noexcept { return acos(x); }

		/// @brief Calculates the arc-tangent of a number
		/// @param x The ratio between the opposite and hypotenuse side lengths
		/// @return The angle (in radians)
		static double Atan(double x) noexcept { return atan(x); }

		/// @brief Calculates the arc-tangent of an (x,y) vector
		/// @param x The x value
		/// @param y The y value
		/// @return The angle (in radians) from the x-axis
		static double Atan2(double x, double y) noexcept { return atan2(x, y); }

		/// @brief Calculates the square root of a number
		/// @param x The number
		/// @return The square root of the number
		static double Sqrt(double x) noexcept { return sqrt(x); }

		/// @brief Calculates the absolute value of an number
		/// @tparam T 
		/// @param x The number
		/// @return The absolute value of the number
		template<typename T>
		static T Abs(const T x) noexcept { return abs(x); }

		/// @brief Determines if the given number is a power of two
		/// @param x The number
		/// @return True if the number is a power of two
		static constexpr bool IsPowerOfTwo(uint64_t x) noexcept { return (x != 0) && ((x & (x - 1)) == 0); }

		/// @brief Compares if two numbers values are within a tolerance of each other. Use this instead of "a == b" to compare decimal numbers due to rounding errors
		/// @tparam T 
		/// @param a The first number
		/// @param b The second number
		/// @param tolerance The acceptable error between the two values
		/// @return True if the numbers are within error of each other
		template<typename T>
		static bool Approximately(const T a, const T b, const T tolerance = Math::Epsilon) noexcept { return Abs(a - b) <= tolerance; }

		/// @brief Returns the greater of two numbers
		/// @tparam T 
		/// @param a The first number
		/// @param b The second number
		/// @return The greater of the two numbers
		template<typename T>
		static constexpr T Max(T a, T b) noexcept { return std::max(a, b); }

		/// @brief Returns the smaller of two numbers
		/// @tparam T 
		/// @param a The first number
		/// @param b The second number
		/// @return The smaller of the two numbers
		template<typename T>
		static constexpr T Min(T a, T b) noexcept { return std::min(a, b); }

		/// @brief Clamps a number between a minimum and a maximum
		/// @tparam T 
		/// @param x The number 
		/// @param minimum The minimum
		/// @param maximum The maximum
		/// @return The number clamped in the range [minimum, maximum]
		template<typename T>
		static constexpr T Clamp(T x, T minimum, T maximum) noexcept { return std::clamp(x, minimum, maximum); }

		/// @brief Rounds a number to the closest non-decimal value
		/// @tparam T 
		/// @param x The number 
		/// @return The rounded number
		template<typename T>
		static T Round(T x) noexcept { return round(x); }

		/// @brief Rounds a number to the closest integer
		/// @tparam T 
		/// @param x The number 
		/// @return The number rounded to an integer
		template<typename T>
		static int RoundToInt(T x) noexcept { return static_cast<int>(Round(x)); }

		/// @brief Gets the largest value of a type
		/// @tparam T 
		/// @return The largest value of the type
		template<typename T>
		static constexpr T MaxValue() noexcept { return std::numeric_limits<T>::max(); }

		/// @brief Gets the smallest value of a type
		/// @tparam T 
		/// @return The smallest value of the type
		template<typename T>
		static constexpr T MinValue() noexcept { return std::numeric_limits<T>::min(); }

		/// @brief Raises a base to the power of an exponent
		/// @tparam T 
		/// @param base The base number
		/// @param exponent The exponent
		/// @return The base raised by the exponent
		template<typename T>
		static constexpr T Pow(T base, T exponent) noexcept { return std::pow(base, exponent); }
	};
}