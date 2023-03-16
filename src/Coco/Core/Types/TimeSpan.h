#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Represents a length of time
	/// </summary>
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

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional days
		/// </summary>
		/// <param name="days">The number of fractional days</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromDays(double days) noexcept { return TimeSpan(static_cast<int64_t>(days) * MicroSecsPerDay); }

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional hours
		/// </summary>
		/// <param name="hours">The number of fractional hours</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromHours(double hours) noexcept { return TimeSpan(static_cast<int64_t>(hours) * MicroSecsPerHour); }

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional minutes
		/// </summary>
		/// <param name="minutes">The number of fractional minutes</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromMinutes(double minutes) noexcept { return TimeSpan(static_cast<int64_t>(minutes) * MicroSecsPerMinute); }

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional seconds
		/// </summary>
		/// <param name="seconds">The number of fractional seconds</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromSeconds(double seconds) noexcept { return TimeSpan(static_cast<int64_t>(seconds) * MicroSecsPerSecond); }

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional milliseconds
		/// </summary>
		/// <param name="milliseconds">The number of fractional milliseconds</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromMilliseconds(double milliseconds) noexcept { return TimeSpan(static_cast<int64_t>(milliseconds) * MicroSecsPerMillisecond); }

		/// <summary>
		/// Gets the number of days in this length of time
		/// </summary>
		/// <returns>The number of days</returns>
		int GetDays() const noexcept { return static_cast<int>(_microseconds / MicroSecsPerDay); }

		/// <summary>
		/// Gets the number of hours in the day for this length of time
		/// </summary>
		/// <returns>The number of hours in the day</returns>
		int GetHours() const noexcept { return static_cast<int>(_microseconds / MicroSecsPerHour % 24); }

		/// <summary>
		/// Gets the number of minutes in the hour for this length of time
		/// </summary>
		/// <returns>The number of minutes in the hour</returns>
		int GetMinutes() const noexcept { return static_cast<int>(_microseconds / MicroSecsPerMinute % 60); }

		/// <summary>
		/// Gets the number of seconds in the minute for this length of time
		/// </summary>
		/// <returns>The number of seconds in the minute</returns>
		int GetSeconds() const noexcept { return static_cast<int>(_microseconds / MicroSecsPerSecond % 60); }

		/// <summary>
		/// Gets the number of milliseconds in the second for this length of time
		/// </summary>
		/// <returns>The number of milliseconds in the second</returns>
		int GetMilliseconds() const noexcept { return static_cast<int>(_microseconds / MicroSecsPerMillisecond % 1000); }

		/// <summary>
		/// Gets the number of microseconds in the millisecond for this length of time
		/// </summary>
		/// <returns>The number of microseconds in the millisecond</returns>
		int GetMicroseconds() const noexcept { return static_cast<int>(_microseconds % MicroSecsPerMillisecond); }

		/// <summary>
		/// Gets the fractional amount of days in this length of time
		/// </summary>
		/// <returns>The fractional amount of days</returns>
		double GetTotalDays() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerDay; }

		/// <summary>
		/// Gets the fractional amount of hours in this length of time
		/// </summary>
		/// <returns>The fractional amount of hours</returns>
		double GetTotalHours() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerHour; }

		/// <summary>
		/// Gets the fractional amount of minutes in this length of time
		/// </summary>
		/// <returns>The fractional amount of minutes</returns>
		double GetTotalMinutes() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerMinute; }

		/// <summary>
		/// Gets the fractional amount of seconds in this length of time
		/// </summary>
		/// <returns>The fractional amount of seconds</returns>
		double GetTotalSeconds() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerSecond; }

		/// <summary>
		/// Gets the fractional amount of milliseconds in this length of time
		/// </summary>
		/// <returns>The fractional amount of milliseconds</returns>
		double GetTotalMilliseconds() const noexcept { return static_cast<double>(_microseconds) / MicroSecsPerMillisecond; }

		/// <summary>
		/// Gets the number of microseconds in this length of time
		/// </summary>
		/// <returns>The amount of microseconds</returns>
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

