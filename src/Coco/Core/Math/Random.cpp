#include "Random.h"
#include <Coco/Core/Engine.h>
#include <Coco/Core/Platform/IEnginePlatform.h>

namespace Coco
{
    bool Random::_isSeeded = false;
    unsigned int Random::_seed = 0;

    int Random::RandomRange(int min, int max)
    {
        EnsureSeeded();

        std::uniform_int_distribution<int> distribution(min, max);

        return distribution(_generator);
    }

    double Random::RandomRange(double min, double max)
    {
        EnsureSeeded();

        std::uniform_real_distribution<double> distribution(min, max);

        return distribution(_generator);
    }

    double Random::RandomValue()
    {
        return RandomRange(0.0, 1.0);
    }

    void Random::SetSeed(unsigned int seed)
    {
        _seed = seed;
        _generator = std::default_random_engine(seed);
        _isSeeded = true;
    }

    unsigned int Random::GetSeed()
    {
        EnsureSeeded();

        return _seed;
    }

    void Random::EnsureSeeded()
    {
        if (_isSeeded)
            return;

        SetSeed(static_cast<unsigned int>(Engine::Get()->GetPlatform()->GetPlatformTimeSeconds()));
    }
}