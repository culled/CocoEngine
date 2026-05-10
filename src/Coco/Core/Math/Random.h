//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_RANDOM_H
#define COCOENGINE_RANDOM_H
#include "Coco/Core/Types/CoreTypes.h"
#include <random>

namespace Coco
{
    /// @brief A pseudo-random number generator
    class Random
    {
    public:
        Random();
        Random(uint32 seed);

        /// @brief Returns a random, 32-bit integer in the range [min, max]
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (inclusive)
        /// @return The random value
        static int RandomInt32(int min, int max) { return _global.GetRandomInt32(min, max); }

        /// @brief Returns a random, 32-bit integer
        /// @return The random value
        static int RandomInt32() { return _global.GetRandomInt32(); }

        /// @brief Returns a random, 64-bit integer in the range [min, max]
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (inclusive)
        /// @return The random value
        static int64 RandomInt64(int64 min, int64 max) { return _global.GetRandomInt64(min, max); }

        /// @brief Returns a random, 64-bit integer
        /// @return The random value
        static int64 RandomInt64() { return _global.GetRandomInt64(); }

        /// @brief Returns a random, 64-bit unsigned integer in the range [min, max]
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (inclusive)
        /// @return The random value
        static uint64 RandomUInt64(uint64 min, uint64 max) { return _global.GetRandomUInt64(min, max); }

        /// @brief Returns a random, 64-bit unsigned integer
        /// @return The random value
        static uint64 RandomUInt64() { return _global.GetRandomUInt64(); }

        /// @brief Returns a random, 32-bit floating point value in the range [min, max)
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (exclusive)
        /// @return The random value
        static float RandomFloat(float min, float max) { return _global.GetRandomFloat(min, max); }

        /// @brief Returns a random, 32-bit floating point value in the range [0, 1)
        /// @return The random value
        static float RandomNormalizedFloat() { return _global.GetRandomNormalizedFloat(); }

        /// @brief Returns a random, 64-bit floating point value in the range [min, max)
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (exclusive)
        /// @return The random value
        static double RandomDouble(double min, double max) { return _global.GetRandomDouble(min, max); }

        /// @brief Returns a random, 64-bit floating point value in the range [0, 1)
        /// @return The random value
        static double RandomNormalizedDouble() { return _global.GetRandomNormalizedDouble(); }

        /// @brief Sets the seed of this pseudo-random number generator. The same seed will always produce the same sequence of numbers
        /// @param seed The seed
        void SetSeed(uint32 seed);

        /// @brief Gets the seed of this pseudo-random number generator
        /// @return The seed value
        uint32 GetSeed() const { return _seed; }

        /// @brief Returns a random, 32-bit integer in the range [min, max]
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (inclusive)
        /// @return The random value
        int GetRandomInt32(int min, int max);

        /// @brief Returns a random, 32-bit integer
        /// @return The random value
        int GetRandomInt32();

        /// @brief Returns a random, 64-bit integer in the range [min, max]
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (inclusive)
        /// @return The random value
        int64 GetRandomInt64(int64 min, int64 max);

        /// @brief Returns a random, 64-bit integer
        /// @return The random value
        int64 GetRandomInt64();

        /// @brief Returns a random, 64-bit unsigned integer in the range [min, max]
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (inclusive)
        /// @return The random value
        uint64 GetRandomUInt64(uint64 min, uint64 max);

        /// @brief Returns a random, 64-bit unsigned integer
        /// @return The random value
        uint64 GetRandomUInt64();

        /// @brief Returns a random, 32-bit floating point value in the range [min, max)
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (exclusive)
        /// @return The random value
        float GetRandomFloat(float min, float max);

        /// @brief Returns a random, 32-bit floating point value in the range [0, 1)
        /// @return The random value
        float GetRandomNormalizedFloat();

        /// @brief Returns a random, 64-bit floating point value in the range [min, max)
        /// @param min The minimum value (inclusive)
        /// @param max The maximum value (exclusive)
        /// @return The random value
        double GetRandomDouble(double min, double max);

        /// @brief Returns a random, 64-bit floating point value in the range [0, 1)
        /// @return The random value
        double GetRandomNormalizedDouble();

    private:
        static Random _global;

        uint32 _seed;
        std::default_random_engine _generator;
    };
} // Coco

#endif //COCOENGINE_RANDOM_H