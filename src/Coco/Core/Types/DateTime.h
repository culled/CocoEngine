#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	struct TimeSpan;

	/// <summary>
	/// Represents a point in time
	/// </summary>
	struct COCOAPI DateTime
	{
	private:
		static const int DaysPerMonth[2][12];
		static const int UnixEpochYear = 1970;

		long long _unixMilliseconds = 0;

	public:
		DateTime(long long unixMilliseconds = 0);
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);

		/// <summary>
		/// Gets if the given year is a leap year
		/// </summary>
		/// <param name="year">The year</param>
		/// <returns>True if the year is a leap year</returns>
		static bool IsLeapYear(int year);

		/// <summary>
		/// Gets the year this DateTime represents
		/// </summary>
		/// <returns>The year</returns>
		int GetYear() const;

		/// <summary>
		/// Gets the month this DateTime represents
		/// </summary>
		/// <returns>The month</returns>
		int GetMonth() const;

		/// <summary>
		/// Gets the day this DateTime represents
		/// </summary>
		/// <returns>The day</returns>
		int GetDay() const;

		/// <summary>
		/// Gets the hour this DateTime represents
		/// </summary>
		/// <returns>The hour</returns>
		int GetHour() const;

		/// <summary>
		/// Gets the minute this DateTime represents
		/// </summary>
		/// <returns>The minute</returns>
		int GetMinute() const;

		/// <summary>
		/// Gets the second this DateTime represents
		/// </summary>
		/// <returns>The second</returns>
		int GetSecond() const;

		/// <summary>
		/// Gets the millisecond this DateTime represents
		/// </summary>
		/// <returns>The millisecond</returns>
		int GetMillisecond() const;

		/// <summary>
		/// Gets the number of hours since unix epoch
		/// </summary>
		/// <returns>The hours since unix epoch</returns>
		double GetTotalHours() const;

		/// <summary>
		/// Gets the number of minutes since unix epoch
		/// </summary>
		/// <returns>The minutes since unix epoch</returns>
		double GetTotalMinutes() const;

		/// <summary>
		/// Gets the number of seconds since unix epoch
		/// </summary>
		/// <returns>The seconds since unix epoch</returns>
		double GetTotalSeconds() const;

		/// <summary>
		/// Gets the number of milliseconds since unix epoch
		/// </summary>
		/// <returns>The milliseconds since unix epoch</returns>
		long long GetTotalMilliseconds() const { return _unixMilliseconds; }

		TimeSpan operator -(const DateTime& other) const;

	private:
		/// <summary>
		/// Calculates the total amount of days since unix epoch for a date
		/// </summary>
		/// <param name="year">The year</param>
		/// <param name="month">The month</param>
		/// <param name="day">The day</param>
		/// <returns>The number of days since epoch the given date is</returns>
		static int DaysSinceEpoch(int year, int month, int day);

		/// <summary>
		/// Calculates the date given the milliseconds since unix epoch
		/// </summary>
		/// <param name="unixMilliseconds">The milliseconds since unix epoch</param>
		/// <param name="year">The year</param>
		/// <param name="month">The month</param>
		/// <param name="day">The day</param>
		static void EpochToYearsMonthDays(long long unixMilliseconds, int* year, int* month, int* day);
	};
}