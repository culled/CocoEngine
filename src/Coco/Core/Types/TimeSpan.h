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
		int64_t _microseconds = 0;

	public:
		TimeSpan() = default;
		TimeSpan(int64_t microseconds) noexcept;

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional days
		/// </summary>
		/// <param name="days">The number of fractional days</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromDays(double days) noexcept;

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional hours
		/// </summary>
		/// <param name="hours">The number of fractional hours</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromHours(double hours) noexcept;

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional minutes
		/// </summary>
		/// <param name="minutes">The number of fractional minutes</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromMinutes(double minutes) noexcept;

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional seconds
		/// </summary>
		/// <param name="seconds">The number of fractional seconds</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromSeconds(double seconds) noexcept;

		/// <summary>
		/// Creates a TimeSpan with the given number of fractional milliseconds
		/// </summary>
		/// <param name="milliseconds">The number of fractional milliseconds</param>
		/// <returns>A TimeSpan</returns>
		static TimeSpan FromMilliseconds(double milliseconds) noexcept;

		/// <summary>
		/// Gets the number of days in this length of time
		/// </summary>
		/// <returns>The number of days</returns>
		int GetDays() const noexcept;

		/// <summary>
		/// Gets the number of hours in the day for this length of time
		/// </summary>
		/// <returns>The number of hours in the day</returns>
		int GetHours() const noexcept;

		/// <summary>
		/// Gets the number of minutes in the hour for this length of time
		/// </summary>
		/// <returns>The number of minutes in the hour</returns>
		int GetMinutes() const noexcept;

		/// <summary>
		/// Gets the number of seconds in the minute for this length of time
		/// </summary>
		/// <returns>The number of seconds in the minute</returns>
		int GetSeconds() const noexcept;

		/// <summary>
		/// Gets the number of milliseconds in the second for this length of time
		/// </summary>
		/// <returns>The number of milliseconds in the second</returns>
		int GetMilliseconds() const noexcept;

		/// <summary>
		/// Gets the number of microseconds in the millisecond for this length of time
		/// </summary>
		/// <returns>The number of microseconds in the millisecond</returns>
		int GetMicroseconds() const noexcept;

		/// <summary>
		/// Gets the fractional amount of days in this length of time
		/// </summary>
		/// <returns>The fractional amount of days</returns>
		double GetTotalDays() const noexcept;

		/// <summary>
		/// Gets the fractional amount of hours in this length of time
		/// </summary>
		/// <returns>The fractional amount of hours</returns>
		double GetTotalHours() const noexcept;

		/// <summary>
		/// Gets the fractional amount of minutes in this length of time
		/// </summary>
		/// <returns>The fractional amount of minutes</returns>
		double GetTotalMinutes() const noexcept;

		/// <summary>
		/// Gets the fractional amount of seconds in this length of time
		/// </summary>
		/// <returns>The fractional amount of seconds</returns>
		double GetTotalSeconds() const noexcept;

		/// <summary>
		/// Gets the fractional amount of milliseconds in this length of time
		/// </summary>
		/// <returns>The fractional amount of milliseconds</returns>
		double GetTotalMilliseconds() const noexcept;

		/// <summary>
		/// Gets the number of microseconds in this length of time
		/// </summary>
		/// <returns>The amount of microseconds</returns>
		int64_t GetTotalMicroseconds() const noexcept { return _microseconds; }

		TimeSpan operator+(const TimeSpan& other) const noexcept;
		TimeSpan operator-(const TimeSpan& other) const noexcept;
		void operator+=(const TimeSpan& other) noexcept;
		void operator-=(const TimeSpan& other) noexcept;

		bool operator<(const TimeSpan& other) noexcept;
		bool operator<=(const TimeSpan& other) noexcept;
		bool operator>(const TimeSpan& other) noexcept;
		bool operator>=(const TimeSpan& other) noexcept;
		bool operator==(const TimeSpan& other) noexcept;
		bool operator!=(const TimeSpan& other) noexcept;
	};
}

