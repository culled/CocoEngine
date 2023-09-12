#pragma once

#include <Coco/Core/Corepch.h>
#include "../Defines.h"

namespace Coco
{
	struct TimeSpan;

	/// @brief Represents a point in time, with millisecond precision
	struct DateTime
	{
		/// @brief The number of milliseconds in a second
		static constexpr int64 MSecsPerSecond = 1000;

		/// @brief The number of milliseconds in a minute
		static constexpr int64 MSecsPerMinute = 60000;

		/// @brief The number of milliseconds in an hour
		static constexpr int64 MSecsPerHour = 3600000;

		/// @brief The number of milliseconds in a day
		static constexpr int64 MSecsPerDay = 86400000;

		/// @brief The epoch year for unix time
		static constexpr int UnixEpochYear = 1970;

		/// @brief The number of milliseconds since unix epoch
		int64 UnixMilliseconds;

		DateTime();
		DateTime(int64 unixMilliseconds);
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);

		virtual ~DateTime() = default;

		/// @brief Gets if the given year is a leap year
		/// @param year The year
		/// @return True if the year is a leap year
		static constexpr bool IsLeapYear(int year) noexcept { return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0); }

		/// @brief Calculates the total amount of days since unix epoch for a date
		/// @param year The year
		/// @param month The month
		/// @param day The day
		/// @return The number of days since epoch the given date is
		static int DaysSinceEpoch(int year, int month, int day) noexcept;

		/// @brief Calculates the date given the milliseconds since unix epoch
		/// @param unixMilliseconds The milliseconds since unix epoch
		/// @param year Will be filled with the year
		/// @param month Will be filled with the month
		/// @param day Will be filled with the day
		static void EpochToYearsMonthDays(int64 unixMilliseconds, int& year, int& month, int& day) noexcept;

		/// @brief Calculates the number of milliseconds from epoch a date is
		/// @param year The year
		/// @param month The month
		/// @param day The day
		/// @param hour The hour
		/// @param minute The minute
		/// @param second The second
		/// @param millisecond The millisecond
		/// @return The milliseconds since unix epoch
		static int64 TimeToUnixMilliseconds(int year, int month, int day, int hour, int minute, int second, int millisecond);

		/// @brief Gets the year this DateTime represents
		/// @return The year
		int GetYear() const;

		/// @brief Gets the month this DateTime represents
		/// @return The month
		int GetMonth() const;

		/// @brief Gets the day this DateTime represents
		/// @return The day
		int GetDay() const;

		/// @brief Gets the hour this DateTime represents
		/// @return The hour
		constexpr int GetHour() const { return static_cast<int>((UnixMilliseconds / MSecsPerHour) % 24); }

		/// @brief Gets the minute this DateTime represents
		/// @return The minute
		constexpr int GetMinute() const { return static_cast<int>((UnixMilliseconds / MSecsPerMinute) % 60); }

		/// @brief Gets the second this DateTime represents
		/// @return The second
		constexpr int GetSecond() const { return static_cast<int>((UnixMilliseconds / MSecsPerSecond) % 60); }

		/// @brief Gets the millisecond this DateTime represents
		/// @return The millisecond
		constexpr int GetMillisecond() const { return static_cast<int>(UnixMilliseconds % MSecsPerSecond); }

		/// @brief Gets the fractional hours since unix epoch
		/// @return The fractional hours
		constexpr double GetTotalHours() const { return static_cast<double>(UnixMilliseconds) / MSecsPerHour; }

		/// @brief Gets the fractional minutes since unix epoch
		/// @return The fractional minutes
		constexpr double GetTotalMinutes() const { return static_cast<double>(UnixMilliseconds) / MSecsPerMinute; }

		/// @brief Gets the fractional seconds since unix epoch
		/// @return The fractional seconds
		constexpr double GetTotalSeconds() const { return static_cast<double>(UnixMilliseconds) / MSecsPerSecond; }

		constexpr auto operator<=>(const DateTime& other) const { return UnixMilliseconds <=> other.UnixMilliseconds; }

		DateTime operator +(const TimeSpan& time) const;
		DateTime operator -(const TimeSpan& time) const;
		void operator +=(const TimeSpan& time);
		void operator -=(const TimeSpan& time);
	};
}