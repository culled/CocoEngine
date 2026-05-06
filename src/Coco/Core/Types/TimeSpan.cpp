//
// Created by cullen on 2/24/26.
//

#include "TimeSpan.h"

namespace Coco
{
    TimeSpan::TimeSpan() noexcept :
        TimeSpan(0)
    {}

    TimeSpan::TimeSpan(int64 microseconds) noexcept :
        Microseconds(microseconds)
    {}

    template<>
    const TimeSpan& Math::Max(const TimeSpan& a, const TimeSpan& b) noexcept
    {
        return a.Microseconds > b.Microseconds ? a : b;
    }

    template<>
    const TimeSpan& Math::Min(const TimeSpan& a, const TimeSpan& b) noexcept
    {
        return a.Microseconds < b.Microseconds ? a : b;
    }
} // Coco