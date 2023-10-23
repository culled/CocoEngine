#pragma once

#include "../Defines.h"

namespace Coco
{
	/// @brief Info for a tick
	struct TickInfo
	{
		/// @brief The number of this tick
		uint64 TickNumber;

		/// @brief The seconds since the last tick, not scaled by the time scale
		double UnscaledDeltaTime;

		/// @brief The current time in seconds, not scaled by the time scale
		double UnscaledTime;

		/// @brief The seconds since the last tick, scaled by the time scale
		double DeltaTime;

		/// @brief The current time in seconds, scaled by the time scale
		double Time;

		TickInfo();
	};
}