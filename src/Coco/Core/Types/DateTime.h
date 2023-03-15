#pragma once

#include <Coco/Core/Core.h>
#include "Array.h"

namespace Coco
{
	struct TimeSpan;

	/// <summary>
	/// Represents a point in time
	/// </summary>
	struct COCOAPI DateTime
	{
	private:
		static constexpr Array<Array<int, 12>, 2> DaysPerMonth = {
			Array<int, 12>({ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }),
			Array<int, 12>({ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })
		};

		static constexpr int UnixEpochYear = 1970;

		int64_t _unixMilliseconds = 0;

	public:
		DateTime() = default;
		DateTime(int64_t unixMilliseconds) noexcept;
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond) noexcept;

		/// <summary>
		/// Gets if the given year is a leap year
		/// </summary>
		/// <param name="year">The year</param>
		/// <returns>True if the year is a leap year</returns>
		static constexpr bool IsLeapYear(int year) noexcept { return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0); }

		/// <summary>
		/// Gets the year this DateTime represents
		/// </summary>
		/// <returns>The year</returns>
		int GetYear() const noexcept;

		/// <summary>
		/// Gets the month this DateTime represents
		/// </summary>
		/// <returns>The month</returns>
		int GetMonth() const noexcept;

		/// <summary>
		/// Gets the day this DateTime represents
		/// </summary>
		/// <returns>The day</returns>
		int GetDay() const noexcept;

		/// <summary>
		/// Gets the hour this DateTime represents
		/// </summary>
		/// <returns>The hour</returns>
		int GetHour() const noexcept;

		/// <summary>
		/// Gets the minute this DateTime represents
		/// </summary>
		/// <returns>The minute</returns>
		int GetMinute() const noexcept;

		/// <summary>
		/// Gets the second this DateTime represents
		/// </summary>
		/// <returns>The second</returns>
		int GetSecond() const noexcept;

		/// <summary>
		/// Gets the millisecond this DateTime represents
		/// </summary>
		/// <returns>The millisecond</returns>
		int GetMillisecond() const noexcept;

		/// <summary>
		/// Gets the number of hours since unix epoch
		/// </summary>
		/// <returns>The hours since unix epoch</returns>
		double GetTotalHours() const noexcept;

		/// <summary>
		/// Gets the number of minutes since unix epoch
		/// </summary>
		/// <returns>The minutes since unix epoch</returns>
		double GetTotalMinutes() const noexcept;

		/// <summary>
		/// Gets the number of seconds since unix epoch
		/// </summary>
		/// <returns>The seconds since unix epoch</returns>
		double GetTotalSeconds() const noexcept;

		/// <summary>
		/// Gets the number of milliseconds since unix epoch
		/// </summary>
		/// <returns>The milliseconds since unix epoch</returns>
		int64_t GetTotalMilliseconds() const noexcept { return _unixMilliseconds; }

		TimeSpan operator -(const DateTime& other) const noexcept;
		DateTime operator +(const TimeSpan& other) const noexcept;
		void operator+=(const TimeSpan& other) noexcept;
		void operator-=(const TimeSpan& other) noexcept;

		bool operator <(const DateTime& other) const noexcept;
		bool operator <=(const DateTime& other) const noexcept;
		bool operator >(const DateTime& other) const noexcept;
		bool operator >=(const DateTime& other) const noexcept;
		bool operator ==(const DateTime& other) const noexcept;
		bool operator !=(const DateTime& other) const noexcept;

	private:
		/// <summary>
		/// Calculates the total amount of days since unix epoch for a date
		/// </summary>
		/// <param name="year">The year</param>
		/// <param name="month">The month</param>
		/// <param name="day">The day</param>
		/// <returns>The number of days since epoch the given date is</returns>
		static int DaysSinceEpoch(int year, int month, int day) noexcept;

		/// <summary>
		/// Calculates the date given the milliseconds since unix epoch
		/// </summary>
		/// <param name="unixMilliseconds">The milliseconds since unix epoch</param>
		/// <param name="year">The year</param>
		/// <param name="month">The month</param>
		/// <param name="day">The day</param>
		static void EpochToYearsMonthDays(int64_t unixMilliseconds, int& year, int& month, int& day) noexcept;
	};
}