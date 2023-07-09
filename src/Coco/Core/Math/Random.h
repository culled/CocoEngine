#pragma once

#include <Coco/Core/Core.h>
#include <random>

namespace Coco
{
	/// @brief A class that provides psuedorandom number generating capabilities
	class COCOAPI Random
	{
	private:
		bool _isSeeded = false;
		uint _seed = 0;
		std::default_random_engine _generator;

	public:
		Random() = default;
		Random(uint seed);

		virtual ~Random() = default;

		/// @brief Generates a psuedorandom value in the range [min, max]. Minimum and maximum are inclusive
		/// @param min The minimum bound (inclusive)
		/// @param max The maximum bound (inclusive)
		/// @return A psuedorandom integer between min and max
		int RandomRange(int min, int max);

		/// @brief Generates a psuedorandom value in the range [min, max). Minimum is inclusive, maximum is exclusive
		/// @param min The minimum bound (inclusive)
		/// @param max The maximum bound (exclusive)
		/// @return A psuedorandom dobule between min and max
		double RandomRange(double min, double max);

		/// @brief Generates a psuedorandom value in the range [0.0, 1.0)
		/// @return A psuedorandom double between 0.0 and 1.0
		double RandomValue();

		/// @brief Sets the seed of this psuedorandom number generator
		/// @param seed The seed for the psuedorandom number generator
		void SetSeed(uint seed);

		/// @brief Gets the current seed used by the psuedorandom number generator
		/// @return The current seed
		uint GetSeed();

	private:
		/// @brief Ensures the psuedorandom number generator has been seeded, either manually or by the clock
		void EnsureSeeded();
	};
}