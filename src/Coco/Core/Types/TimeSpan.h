#pragma once

#include <Coco/Core/API.h>

namespace Coco
{
	/// @brief Represents a length of time
	struct COCOAPI TimeSpan
	{
	private:
		static constexpr int64_t MicroSecsPerMillisecond = 1000;
		static constexpr int64_t MicroSecsPerSecond = 1000000;
		static constexpr int64_t MicroSecsPerMinute = 60000000;
		static constexpr int64_t MicroSecsPerHour = 3600000000;
		static constexpr int64_t MicroSecsPerDay = 86400000000;

		int64_t _microseconds = 0;

	public:
		TimeSpan() noexcept = default;
		TimeSpan(int64_t microseconds) noexcept;
		virtual ~TimeSpan() = default;

		/// @brief Creates a TimeSpan with the given number of fractional days
		/// @param days The number of fractional days
		/// @return A TimeSpan
		static TimeSpan FromDays(double days) noexcept { return TimeSpan(static_cast<int64_t>(days) * MicroSecsPerDay); }

		/// @brief Creates a TimeSpan with the given number of fractional hours
		/// @param hours The number of fractional hours
		/// @return A TimeSpan
		static TimeSpan FromHours(double hours) noexcept { return TimeSpan(static_cast<int64_t>(hours) * MicroSecsPerHour); }

		/// @brief Creates a TimeSpan with the given number of fractional minutes
		/// @param minutes The number of fractional minutes
		/// @return A Timespan
		static TimeSpan FromMinutes(double minutes) noexcept { return TimeSpan(static_cast<int64_t>(minutes) * MicroSecsPerMinute); }

		/// @brief Creates a TimeSpan with the given number of fractional seconds
		/// @param seconds The number of fractional seconds
		/// @return A TimeSpan
		static TimeSpan FromSeconds(double seconds) noexcept { return TimeSpan(static_cast<int64_t>(seconds) * MicroSecsPerSecond); }

		/// @brief Creates a TimeSpan with the given number of fractional milliseconds
		/// @param milliseconds The number of fractional milliseconds
		/// @return A TimeSpan
		static TimeSpan FromMilliseconds(double milliseconds) noexcept { return TimeSpan(static_cast<int64_t>(milliseconds) * MicroSecsPerMillisecond); }

		/// @brief Gets the number of days in this length of time
		/// @return The number of days
		int64_t GetDays() const noexcept { return _microseconds / MicroSecsPerDay; }

		/// @brief Gets the number of hours in this length of time
		/// @return The number of hours
		int64_t GetHours() const noexcept { return _microseconds / MicroSecsPerHour % 24; }

		/// @brief Gets the number of minutes in this length of time
		/// @return The number of minutes
		int64_t GetMinutes() const noexcept { return _microseconds / MicroSecsPerMinute % 60; }

		/// @brief Gets the number of seconds in this length of time
		/// @return The number of seconds
		int64_t GetSeconds() const noexcept { return _microseconds / MicroSecsPerSecond % 60; }

		/// @brief Gets the number of milliseconds in this length of time
		/// @return The number of milliseconds
		int64_t GetMilliseconds() const noexcept { return _microseconds / MicroSecsPerMillisecond % 1000; }

		/// @brief Gets the number of microseconds in this length of time
		/// @return The number of microseconds
		int64_t GetMicroseconds() const noexcept { return _microseconds % MicroSecsPerMillisecond; }

		/// @brief Gets the fractional amount of days in this length of time
		/// @return The fractional amount of days
		double GetTotalDays() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerDay; }

		/// @brief Gets the fractional amount of hours in this length of time
		/// @return The fractional amount of hours
		double GetTotalHours() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerHour; }

		/// @brief Gets the fractional amount of minutes in this length of time
		/// @return The fractional amount of minutes
		double GetTotalMinutes() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerMinute; }

		/// @brief Gets the fractional amount of seconds in this length of time
		/// @return The fractional amount of seconds
		double GetTotalSeconds() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerSecond; }

		/// @brief Gets the fractional amount of milliseconds in this length of time
		/// @return The fractional amount of milliseconds
		double GetTotalMilliseconds() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerMillisecond; }

		/// @brief Gets the number of microseconds in this length of time
		/// @return The number of microseconds
		int64_t GetTotalMicroseconds() const noexcept { return _microseconds; }

		TimeSpan operator+(const TimeSpan& other) const noexcept { return TimeSpan(_microseconds + other._microseconds); }
		TimeSpan operator-(const TimeSpan& other) const noexcept { return TimeSpan(_microseconds - other._microseconds); }
		void operator+=(const TimeSpan& other) noexcept { _microseconds += other._microseconds; }
		void operator-=(const TimeSpan& other) noexcept { _microseconds -= other._microseconds; }

		bool operator<(const TimeSpan& other) noexcept { return _microseconds < other._microseconds; }
		bool operator<=(const TimeSpan& other) noexcept { return _microseconds <= other._microseconds; }
		bool operator>(const TimeSpan& other) noexcept { return _microseconds > other._microseconds; }
		bool operator>=(const TimeSpan& other) noexcept { return _microseconds >= other._microseconds; }
		bool operator==(const TimeSpan& other) noexcept { return _microseconds == other._microseconds; }
		bool operator!=(const TimeSpan& other) noexcept { return _microseconds != other._microseconds; }
	};
}

