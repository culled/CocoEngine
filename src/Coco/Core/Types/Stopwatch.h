#pragma once

#include "TimeSpan.h"

namespace Coco
{
	/// @brief A basic stopwatch that can be started and stopped
	class Stopwatch
	{
	public:
		Stopwatch();
		Stopwatch(bool startAutomatically);

		/// @brief Starts this timer
		void Start();

		/// @brief Stops this timer and returns the duration of time it was running
		/// @return The time since this timer was started
		const TimeSpan& Stop();

		/// @brief Gets the duration of time that this timer was running.
		/// NOTE: Only valid after calling Stop()
		/// @return The duration of time
		const TimeSpan& GetDuration() const { return _time; }

		/// @brief Gets the time that this timer was last started, in platform seconds
		/// @return The time that this timer was last started
		double GetStartTimeSeconds() const { return _startTimeSeconds; }

	private:
		TimeSpan _time;
		double _startTimeSeconds;
		bool _isRunning;
	};
}