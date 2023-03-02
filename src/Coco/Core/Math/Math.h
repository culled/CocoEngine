#pragma once

#include <Coco/Core/Core.h>

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
		static double Deg2Rad(double degrees) { return degrees * Deg2RadMultiplier; }
		static double Rad2Deg(double radians) { return radians * Rad2DegMultiplier; }

		static double Sin(double x);
		static double Cos(double x);
		static double Tan(double x);
		static double ACos(double x);

		static double Sqrt(double x);

		static double Abs(double x);
		static int Abs(int x);

		/// <summary>
		/// Determines if the given value is a power of two
		/// </summary>
		/// <param name="value>The value</param>
		/// <returns>True if the number is a power of two</returns>
		static bool IsPowerOfTwo(unsigned long long value) { return (value != 0) && ((value & (value - 1)) == 0); }

		static bool Approximately(double a, double b, double tolerance = Math::Epsilon);
	};
}