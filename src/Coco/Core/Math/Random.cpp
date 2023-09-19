#include "Corepch.h"
#include "Random.h"
#include "Math.h"

#include "../Engine.h"

namespace Coco
{
    Random Random::Global = Random();

    Random::Random()
    {
        EnsureSeeded();
    }

    Random::Random(uint32 seed)
    {
        SetSeed(seed);
    }

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

    void Random::SetSeed(uint32 seed)
    {
        _seed = seed;
        _generator = std::default_random_engine(_seed);
        _isSeeded = true;
    }

    uint32 Random::GetSeed()
    {
        EnsureSeeded();

        return _seed;
    }

    void Random::EnsureSeeded()
    {
        if (_isSeeded)
            return;

        uint32 seed = 0;

        if (Engine::cGet())
        {
            double time = Math::Max(Engine::cGet()->GetPlatform()->GetRunningTime(), 0.0);
            seed = static_cast<uint32>(Math::Round(time));
        }
        
        SetSeed(seed);
    }
}