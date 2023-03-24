#include "DateTime.h"

#include <Coco/Core/Types/TimeSpan.h>

namespace Coco
{
	DateTime::DateTime(int64_t unixMilliseconds) noexcept :
		_unixMilliseconds(unixMilliseconds)
	{}

	DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond) noexcept
	{
		const int days = DaysSinceEpoch(year, month, day);

		_unixMilliseconds = days * MSecsPerDay +
			hour * MSecsPerHour +
			minute * MSecsPerMinute +
			second * MSecsPerSecond +
			millisecond;
	}

	int DateTime::GetYear() const noexcept
	{
		int year;
		int month;
		int day;

		EpochToYearsMonthDays(_unixMilliseconds, year, month, day);

		return year;
	}

	int DateTime::GetMonth() const noexcept
	{
		int year;
		int month;
		int day;

		EpochToYearsMonthDays(_unixMilliseconds, year, month, day);

		return month;
	}

	int DateTime::GetDay() const noexcept
	{
		int year;
		int month;
		int day;

		EpochToYearsMonthDays(_unixMilliseconds, year, month, day);

		return day;
	}

	TimeSpan DateTime::operator -(const DateTime& other) const noexcept { return TimeSpan((_unixMilliseconds - other._unixMilliseconds) * 1000); }
	DateTime DateTime::operator +(const TimeSpan& timeSpan) const noexcept { return DateTime(_unixMilliseconds + static_cast<int64_t>(timeSpan.GetTotalMilliseconds())); }
	void DateTime::operator+=(const TimeSpan& timeSpan) noexcept { _unixMilliseconds += static_cast<int64_t>(timeSpan.GetTotalMilliseconds()); }
	void DateTime::operator-=(const TimeSpan& timeSpan) noexcept { _unixMilliseconds -= static_cast<int64_t>(timeSpan.GetTotalMilliseconds()); }

	int DateTime::DaysSinceEpoch(int year, int month, int day) noexcept
	{
		// Since the epoch is 0 indexed, we need to subtract by 1 day
		int dayCount = day - 1;

		const int leapYearIndex = IsLeapYear(year) ? 1 : 0;

		// Add days in each month
		for (int m = 0; m < month - 1; m++)
		{
			dayCount += DaysPerMonth.at(leapYearIndex).at(m);
		}

		// Number of days is positive for dates on or after epoch, negative for dates before epoch
		if (year >= UnixEpochYear)
		{
			for (int y = UnixEpochYear; y < year; y++)
			{
				dayCount += IsLeapYear(y) ? 366 : 365;
			}
		}
		else
		{
			for (int y = UnixEpochYear - 1; y >= year; y--)
			{
				dayCount -= IsLeapYear(y) ? 366 : 365;
			}
		}

		return dayCount;
	}

	void DateTime::EpochToYearsMonthDays(int64_t unixMilliseconds, int& year, int& month, int& day) noexcept
	{
		const int64_t daysSinceEpoch = unixMilliseconds / MSecsPerDay;
		int currentYear = UnixEpochYear;
		int currentDay = static_cast<int>(daysSinceEpoch);

		// Figure out the year. Current day is the 0 indexed day since epoch
		if (daysSinceEpoch >= 0)
		{
			while (currentDay > (IsLeapYear(currentYear) ? 366 : 365))
			{
				currentDay -= IsLeapYear(currentYear) ? 366 : 365;
				currentYear++;
			}
		}
		else
		{
			while (currentDay < 0)
			{
				currentDay += IsLeapYear(currentYear) ? 366 : 365;
				currentYear--;
			}
		}

		year = currentYear;

		// Now figure out the month. Current day is the 0 indexed day of the year
		const int leapYearIndex = IsLeapYear(currentYear) ? 1 : 0;
		int currentMonthIndex = 0;

		while (currentDay >= DaysPerMonth.at(leapYearIndex).at(currentMonthIndex))
		{
			currentDay -= DaysPerMonth.at(leapYearIndex).at(currentMonthIndex);
			currentMonthIndex++;
		}

		// Month and day are not 0 indexed, so add 1 to them
		month = currentMonthIndex + 1;
		day = currentDay + 1;
	}
}