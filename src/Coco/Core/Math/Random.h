//
// Created by cullen on 4/1/26.
//

#ifndef COCOENGINE_RANDOM_H
#define COCOENGINE_RANDOM_H
#include "Coco/Core/Types/CoreTypes.h"
#include <random>

namespace Coco
{
    class Random
    {
    public:
        Random();
        Random(uint32 seed);

        static int RandomInt32(int min, int max) { return _global.GetRandomInt32(min, max); }
        static int RandomInt32() { return _global.GetRandomInt32(); }
        static int64 RandomInt64(int64 min, int64 max) { return _global.GetRandomInt64(min, max); }
        static int64 RandomInt64() { return _global.GetRandomInt64(); }
        static uint64 RandomUInt64(uint64 min, uint64 max) { return _global.GetRandomUInt64(min, max); }
        static uint64 RandomUInt64() { return _global.GetRandomUInt64(); }
        static float RandomFloat(float min, float max) { return _global.GetRandomFloat(min, max); }
        static float RandomNormalizedFloat() { return _global.GetRandomNormalizedFloat(); }
        static double RandomDouble(double min, double max) { return _global.GetRandomDouble(min, max); }
        static double RandomNormalizedDouble() { return _global.GetRandomNormalizedDouble(); }

        void SetSeed(uint32 seed);
        uint32 GetSeed() const { return _seed; }

        int GetRandomInt32(int min, int max);
        int GetRandomInt32();
        int64 GetRandomInt64(int64 min, int64 max);
        int64 GetRandomInt64();
        uint64 GetRandomUInt64(uint64 min, uint64 max);
        uint64 GetRandomUInt64();

        float GetRandomFloat(float min, float max);
        float GetRandomNormalizedFloat();

        double GetRandomDouble(double min, double max);
        double GetRandomNormalizedDouble();

    private:
        static Random _global;

        uint32 _seed;
        std::default_random_engine _generator;
    };
} // Coco

#endif //COCOENGINE_RANDOM_H