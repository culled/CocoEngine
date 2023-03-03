#pragma once

#include <Coco/Core/Core.h>
#include <random>

namespace Coco
{
	class COCOAPI Random
	{
	private:
		static bool _isSeeded;
		static unsigned int _seed;

		static std::default_random_engine _generator;

	public:
		/// <summary>
		/// Generates a psuedorandom value in the range [min, max].
		/// Minimum and maximum are inclusive
		/// </summary>
		/// <param name="min">The minimum bound (inclusive)</param>
		/// <param name="max">The maximum bound (inclusive)</param>
		/// <returns>A psuedorandom integer between min and max</returns>
		static int RandomRange(int min, int max);

		/// <summary>
		/// Generates a psuedorandom value in the range [min, max).
		/// Minimum is inclusive, maximum is exclusive
		/// </summary>
		/// <param name="min">The minimum bound (inclusive)</param>
		/// <param name="max">The maximum bound (exclusive)</param>
		/// <returns>A psuedorandom dobule between min and max</returns>
		static double RandomRange(double min, double max);

		/// <summary>
		/// Generates a psuedorandom value in the range [0.0, 1.0)
		/// </summary>
		/// <returns>A psuedorandom double between 0.0 and 1.0</returns>
		static double RandomValue();

		/// <summary>
		/// Sets the seed of the psuedorandom number generator
		/// </summary>
		/// <param name="seed">The seed for the psuedorandom number generator</param>
		static void SetSeed(unsigned int seed);

		/// <summary>
		/// Gets the current seed used by the psuedorandom number generator
		/// </summary>
		/// <returns>The current seed</returns>
		static unsigned int GetSeed();

	private:
		/// <summary>
		/// Ensures the psuedorandom number generator has been seeded, either manually or by the clock
		/// </summary>
		static void EnsureSeeded();
	};
}