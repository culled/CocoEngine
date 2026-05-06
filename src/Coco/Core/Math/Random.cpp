//
// Created by cullen on 4/1/26.
//

#include "Random.h"

#include "Coco/Core/Engine.h"

namespace Coco
{
    Random Random::_global = Random();

    Random::Random() :
        _seed(Engine::Get() ? Engine::Get()->GetPlatform()->GetRunningTime().Microseconds : 0),
        _generator(_seed)
    {}

    Random::Random(uint32 seed) :
        _seed(seed),
        _generator(seed)
    {}

    void Random::SetSeed(uint32 seed)
    {
        _seed = seed;
        _generator.seed(_seed);
    }

    int Random::GetRandomInt32(int min, int max)
    {
        std::uniform_int_distribution distribution(min, max);
        return distribution(_generator);
    }

    int Random::GetRandomInt32()
    {
        return GetRandomInt32(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    }

    int64 Random::GetRandomInt64(int64 min, int64 max)
    {
        std::uniform_int_distribution distribution(min, max);
        return distribution(_generator);
    }

    int64 Random::GetRandomInt64()
    {
        return GetRandomInt64(std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max());
    }

    uint64 Random::GetRandomUInt64(uint64 min, uint64 max)
    {
        std::uniform_int_distribution distribution(min, max);
        return distribution(_generator);
    }

    uint64 Random::GetRandomUInt64()
    {
        return GetRandomUInt64(0, std::numeric_limits<uint64>::max());
    }

    float Random::GetRandomFloat(float min, float max)
    {
        std::uniform_real_distribution distribution(min, max);
        return distribution(_generator);
    }

    float Random::GetRandomNormalizedFloat()
    {
        return GetRandomFloat(0.0f, 1.0f);
    }

    double Random::GetRandomDouble(double min, double max)
    {
        std::uniform_real_distribution distribution(min, max);
        return distribution(_generator);
    }

    double Random::GetRandomNormalizedDouble()
    {
        return GetRandomDouble(0.0, 1.0);
    }
} // Coco