#pragma once

#include <Coco/Core/Core.h>

#include <type_traits>

namespace Coco
{
	class COCOAPI Math
	{
	public:
		static const double PI;
		static const double DoublePI;
		static const double HalfPI;
		static const double QuarterPI;
		static const double OneOverPI;
		static const double OneOverDoublePI;
		static const double SqrtTwo;
		static const double SqrtThree;
		static const double SqrtOneOverTwo;
		static const double SqrtOneOverThree;
		static const double Deg2RadMultiplier;
		static const double Rad2DegMultiplier;

		static const double SecondsToMillisecondsMultiplier;
		static const double MillisecondsToSecondsMultiplier;

		static const double Infinity;
		static const double Epsilon;

	public:
		/// <summary>
		/// Converts degrees to radians
		/// </summary>
		/// <param name="degrees">The degrees</param>
		/// <returns>The equivalent radians</returns>
		static constexpr double Deg2Rad(double degrees) { return degrees * Deg2RadMultiplier; }

		/// <summary>
		/// Converts radians to degrees
		/// </summary>
		/// <param name="radians">The radians</param>
		/// <returns>The equivalent degrees</returns>
		static constexpr double Rad2Deg(double radians) { return radians * Rad2DegMultiplier; }

		/// <summary>
		/// Calculates the sine of an angle
		/// </summary>
		/// <param name="x">The angle (in radians)</param>
		/// <returns>The sine</returns>
		static double Sin(double x) { return sin(x); }

		/// <summary>
		/// Calculates the cosine of an angle
		/// </summary>
		/// <param name="x">The angle (in radians)</param>
		/// <returns>The cosine</returns>
		static double Cos(double x) { return cos(x); }

		/// <summary>
		/// Calculates the tangent of an angle
		/// </summary>
		/// <param name="x">The angle (in radians)</param>
		/// <returns>The tangent</returns>
		static double Tan(double x) { return tan(x); }

		/// <summary>
		/// Calculates the arc-sine of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The angle (in radians)</returns>
		static double ASin(double x) { return asin(x); }

		/// <summary>
		/// Calculates the arc-cosine of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The angle (in radians)</returns>
		static double ACos(double x) { return acos(x); }

		/// <summary>
		/// Calculates the arc-tangent of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The angle (in radians)</returns>
		static double ATan(double x) { return atan(x); }

		/// <summary>
		/// Calculates the arc-tangent of an (x,y) vector
		/// </summary>
		/// <param name="x">The x value</param>
		/// <param name="y">The y value</param>
		/// <returns>The angle (in radians) from the x-axis</returns>
		static double ATan2(double x, double y) { return atan2(x, y); }

		/// <summary>
		/// Calculates the square root of a number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The square root</returns>
		static double Sqrt(double x) { return sqrt(x); }

		/// <summary>
		/// Calculates the absolute value of an number
		/// </summary>
		/// <param name="x">The number</param>
		/// <returns>The absolute value of the number</returns>
		template<typename T>
		static T Abs(T x) { return abs(x); }

		/// <summary>
		/// Determines if the given value is a power of two
		/// </summary>
		/// <param name="value>The value</param>
		/// <returns>True if the number is a power of two</returns>
		static bool IsPowerOfTwo(uint64_t value) { return (value != 0) && ((value & (value - 1)) == 0); }

		/// <summary>
		/// Compares if two decimal values are within a tolerance of each other.
		/// Use this instead of "a == b" to compare decimals due to rounding errors
		/// </summary>
		/// <param name="a">The first number</param>
		/// <param name="b">The second number</param>
		/// <param name="tolerance">The acceptable error between the two values</param>
		/// <returns>True if the values are within error of each other</returns>
		template<typename T>
		static bool Approximately(T a, T b, T tolerance = Math::Epsilon) { return Abs(a - b) <= tolerance; }

		/// <summary>
		/// Returns the greater of two numbers
		/// </summary>
		/// <returns>The greater of the two numbers</returns>
		template<typename T>
		static T Max(T a, T b) { return std::max(a, b); }

		/// <summary>
		/// Returns the smaller of two numbers
		/// </summary>
		/// <returns>The smaller of the two numbers</returns>
		template<typename T>
		static T Min(T a, T b) { return std::min(a, b); }

		/// <summary>
		/// Clamps a value between a minimum and a maximum
		/// </summary>
		/// <param name="value">The value</param>
		/// <param name="minimum">The minimum</param>
		/// <param name="maximum">The maximum</param>
		/// <returns>The value clamped in the range [minimum, maximum]</returns>
		template<typename T>
		static T Clamp(T value, T minimum, T maximum) { return std::clamp(value, minimum, maximum); }
	};
}