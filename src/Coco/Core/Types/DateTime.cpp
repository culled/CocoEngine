#include "Corepch.h"
#include "DateTime.h"
#include "TimeSpan.h"

namespace Coco
{
	/// @brief The days in each month. Index 0 is a non-leap year, index 1 is a leap year
	constexpr std::array<std::array<int, 12>, 2> DaysPerMonth = {
		std::array<int, 12>({ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }),
		std::array<int, 12>({ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })
	};


	DateTime::DateTime() :
		UnixMilliseconds(0)
	{}

	DateTime::DateTime(int64 unixMilliseconds) :
		UnixMilliseconds(unixMilliseconds)
	{}

	DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond) :
		UnixMilliseconds(TimeToUnixMilliseconds(year, month, day, hour, minute, second, millisecond))
	{}

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

	void DateTime::EpochToYearsMonthDays(int64 unixMilliseconds, int& year, int& month, int& day) noexcept
	{
		const int64 daysSinceEpoch = unixMilliseconds / MSecsPerDay;
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

	int64 DateTime::TimeToUnixMilliseconds(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		int days = DaysSinceEpoch(year, month, day);

		return days * MSecsPerDay + hour * MSecsPerHour + minute * MSecsPerMinute + second * MSecsPerSecond + millisecond;
	}

	int DateTime::GetYear() const
	{
		int year, month, day;
		EpochToYearsMonthDays(UnixMilliseconds, year, month, day);

		return year;
	}

	int DateTime::GetMonth() const
	{
		int year, month, day;
		EpochToYearsMonthDays(UnixMilliseconds, year, month, day);

		return month;
	}

	int DateTime::GetDay() const
	{
		int year, month, day;
		EpochToYearsMonthDays(UnixMilliseconds, year, month, day);

		return day;
	}

	DateTime DateTime::operator+(const TimeSpan& time) const
	{
		return DateTime(UnixMilliseconds + static_cast<int64>(Math::Round(time.GetMilliseconds())));
	}

	DateTime DateTime::operator-(const TimeSpan& time) const
	{
		return DateTime(UnixMilliseconds - static_cast<int64>(Math::Round(time.GetMilliseconds())));
	}

	void DateTime::operator+=(const TimeSpan& time)
	{
		UnixMilliseconds += static_cast<int64>(Math::Round(time.GetMilliseconds()));
	}

	void DateTime::operator-=(const TimeSpan& time)
	{
		UnixMilliseconds -= static_cast<int64>(Math::Round(time.GetMilliseconds()));
	}
}