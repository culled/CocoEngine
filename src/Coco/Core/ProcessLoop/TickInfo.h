//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_TICKINFO_H
#define COCOENGINE_TICKINFO_H
#include "../Types/TimeSpan.h"

namespace Coco
{
    /// @brief Information for a tick
    struct TickInfo
    {
        /// @brief The tick number
        uint64 TickNumber;

        /// @brief The time between this tick and the last tick, scaled by the time scale 
        TimeSpan DeltaTime;

        /// @brief The time since the Engine started, scaled by the time scale 
        TimeSpan Time;

        /// @brief The time between this tick and the last tick 
        TimeSpan UnscaledDeltaTime;

        /// @brief The time since the Engine started 
        TimeSpan UnscaledTime;
    };
}

#endif //COCOENGINE_TICKINFO_H