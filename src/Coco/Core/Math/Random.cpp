#include "Random.h"
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/IEnginePlatform.h>

namespace Coco
{
    Random::Random(uint seed)
    {
        SetSeed(seed);
    }

    int Random::RandomRange(int min, int max)
    {
        EnsureSeeded();

        const std::uniform_int_distribution<int> distribution(min, max);

        return distribution(_generator);
    }

    double Random::RandomRange(double min, double max)
    {
        EnsureSeeded();

        const std::uniform_real_distribution<double> distribution(min, max);

        return distribution(_generator);
    }

    double Random::RandomValue()
    {
        return RandomRange(0.0, 1.0);
    }

    void Random::SetSeed(uint seed)
    {
        _seed = seed;
        _generator = std::default_random_engine(_seed);
        _isSeeded = true;
    }

    uint Random::GetSeed()
    {
        EnsureSeeded();

        return _seed;
    }

    void Random::EnsureSeeded()
    {
        if (_isSeeded)
            return;

        const double time = Math::Max(Engine::Get()->GetPlatform()->GetRunningTimeSeconds(), 0.0);
        const int seed = Math::RoundToInt(time);
        SetSeed(seed);
    }
}