#pragma once

#include <Coco/Core/API.h>

#include "Array.h"

namespace Coco
{
	struct TimeSpan;

	/// @brief Represents a point in time
	struct COCOAPI DateTime
	{
		/// @brief The days in each month. Index 0 is a non-leap year, index 1 is a leap year
		static constexpr Array<Array<int, 12>, 2> DaysPerMonth = {
			Array<int, 12>({ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }),
			Array<int, 12>({ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })
		};

		/// @brief The number of milliseconds in a second
		static constexpr int64_t MSecsPerSecond = 1000;

		/// @brief The number of milliseconds in a minute
		static constexpr int64_t MSecsPerMinute = 60000;

		/// @brief The number of milliseconds in an hour
		static constexpr int64_t MSecsPerHour = 3600000;

		/// @brief The number of milliseconds in a day
		static constexpr int64_t MSecsPerDay = 86400000;

		/// @brief The epoch year for unix time
		static constexpr int UnixEpochYear = 1970;

	public:
		int64_t _unixMilliseconds = 0;

	public:
		DateTime() noexcept = default;
		DateTime(int64_t unixMilliseconds) noexcept;
		DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond) noexcept;
		virtual ~DateTime() = default;

		/// @brief Gets if the given year is a leap year
		/// @param year The year
		/// @return True if the year is a leap year
		static constexpr bool IsLeapYear(int year) noexcept { return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0); }

		/// @brief Gets the year this DateTime represents
		/// @return The year
		int GetYear() const noexcept;

		/// @brief Gets the month this DateTime represents
		/// @return The month
		int GetMonth() const noexcept;

		/// @brief Gets the day this DateTime represents
		/// @return The day
		int GetDay() const noexcept;

		/// @brief Gets the hour this DateTime represents
		/// @return The hour
		int64_t GetHour() const noexcept { return _unixMilliseconds / MSecsPerHour % 24; }

		/// @brief Gets the minute this DateTime represents
		/// @return The minute
		int64_t GetMinute() const noexcept { return _unixMilliseconds / MSecsPerMinute % 60; }

		/// @brief Gets the second this DateTime represents
		/// @return The second
		int64_t GetSecond() const noexcept { return _unixMilliseconds / MSecsPerSecond % 60; }

		/// @brief Gets the millisecond this DateTime represents
		/// @return The millisecond
		int64_t GetMillisecond() const noexcept { return _unixMilliseconds % 1000; }

		/// @brief Gets the number of hours since unix epoch
		/// @return The hours since unix epoch
		double GetTotalHours() const noexcept { return static_cast<double>(_unixMilliseconds) / MSecsPerHour; }

		/// @brief Gets the number of minutes since unix epoch
		/// @return The minutes since unix epoch
		double GetTotalMinutes() const noexcept { return static_cast<double>(_unixMilliseconds) / MSecsPerMinute; }

		/// @brief Gets the number of seconds since unix epoch
		/// @return The seconds since unix epoch
		double GetTotalSeconds() const noexcept { return static_cast<double>(_unixMilliseconds) / MSecsPerSecond; }

		/// @brief Gets the number of milliseconds since unix epoch
		/// @return The milliseconds since unix epoch
		constexpr int64_t GetTotalMilliseconds() const noexcept { return _unixMilliseconds; }

		TimeSpan operator -(const DateTime& other) const noexcept;
		DateTime operator +(const TimeSpan& timeSpan) const noexcept;
		void operator+=(const TimeSpan& timeSpan) noexcept;
		void operator-=(const TimeSpan& timeSpan) noexcept;

		bool operator <(const DateTime& other) const noexcept {	return _unixMilliseconds < other._unixMilliseconds; }
		bool operator <=(const DateTime& other) const noexcept { return _unixMilliseconds <= other._unixMilliseconds; }
		bool operator >(const DateTime& other) const noexcept { return _unixMilliseconds > other._unixMilliseconds; }
		bool operator >=(const DateTime& other) const noexcept { return _unixMilliseconds >= other._unixMilliseconds; }
		bool operator ==(const DateTime& other) const noexcept { return _unixMilliseconds == other._unixMilliseconds; }
		bool operator !=(const DateTime& other) const noexcept { return _unixMilliseconds != other._unixMilliseconds; }

	private:
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
		static void EpochToYearsMonthDays(int64_t unixMilliseconds, int& year, int& month, int& day) noexcept;
	};
}