#pragma once

#include <Coco/Core/Corepch.h>
#include "../Defines.h"

namespace Coco
{
	/// @brief Represents a duration of time, with microsecond precision
	struct TimeSpan
	{
		/// @brief The number of microseconds in a millisecond
		static constexpr int64 MicroSecsPerMillisecond = 1000;

		/// @brief The number of microseconds in a second
		static constexpr int64 MicroSecsPerSecond = 1000000;

		/// @brief The number of microseconds in a minute
		static constexpr int64 MicroSecsPerMinute = 60000000;

		/// @brief The number of microseconds in an hour
		static constexpr int64 MicroSecsPerHour = 3600000000;

		/// @brief The number of microseconds in a day
		static constexpr int64 MicroSecsPerDay = 86400000000;

		/// @brief The number of microseconds
		int64 Microseconds = 0;

		TimeSpan();
		TimeSpan(int64 microseconds);

		virtual ~TimeSpan() = default;

		/// @brief Creates a TimeSpan with the given number of fractional days
		/// @param days The number of fractional days
		/// @return A TimeSpan
		static TimeSpan FromDays(double days) { return TimeSpan(static_cast<int64>(round(days * MicroSecsPerDay))); }

		/// @brief Creates a TimeSpan with the given number of fractional hours
		/// @param hours The number of fractional hours
		/// @return A TimeSpan
		static TimeSpan FromHours(double hours) { return TimeSpan(static_cast<int64>(round(hours * MicroSecsPerHour))); }

		/// @brief Creates a TimeSpan with the given number of fractional seconds
		/// @param seconds The number of fractional seconds
		/// @return A TimeSpan
		static TimeSpan FromSeconds(double seconds) { return TimeSpan(static_cast<int64>(round(seconds * MicroSecsPerSecond))); }

		/// @brief Creates a TimeSpan with the given number of fractional milliseconds
		/// @param milliseconds The number of fractional milliseconds
		/// @return A TimeSpan
		static TimeSpan FromMilliseconds(double milliseconds) { return TimeSpan(static_cast<int64>(round(milliseconds * MicroSecsPerMillisecond))); }

		/// @brief Gets the number of days in this TimeSpan
		/// @return The number of days
		constexpr double GetDays() const { return static_cast<double>(Microseconds) / MicroSecsPerDay; }

		/// @brief Gets the number of hours in this TimeSpan
		/// @return The number of hours
		constexpr double GetHours() const { return static_cast<double>(Microseconds) / MicroSecsPerHour; }

		/// @brief Gets the number of minutes in this TimeSpan
		/// @return The number of minutes
		constexpr double GetMinutes() const { return static_cast<double>(Microseconds) / MicroSecsPerMinute; }

		/// @brief Gets the number of seconds in this TimeSpan
		/// @return The number of seconds
		constexpr double GetSeconds() const { return static_cast<double>(Microseconds) / MicroSecsPerSecond; }

		/// @brief Gets the number of milliseconds in this TimeSpan
		/// @return The number of milliseconds
		constexpr double GetMilliseconds() const { return static_cast<double>(Microseconds) / MicroSecsPerMillisecond; }

		TimeSpan operator+(const TimeSpan& other) { return TimeSpan(Microseconds + other.Microseconds); }
		TimeSpan operator-(const TimeSpan& other) { return TimeSpan(Microseconds - other.Microseconds); }
		void operator+=(const TimeSpan& other) { Microseconds += other.Microseconds; }
		void operator-=(const TimeSpan& other) { Microseconds -= other.Microseconds; }
	};
}