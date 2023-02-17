#include "DateTime.h"

#include <Coco/Core/Types/TimeSpan.h>

namespace Coco
{
	const int DateTime::DaysPerMonth[2][12] = {
		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
		{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	};

	const long long MSecsPerSecond = 1000;
	const long long MSecsPerMinute = 60000;
	const long long MSecsPerHour = 3600000;
	const long long MSecsPerDay = 86400000;

	DateTime::DateTime(long long unixMilliseconds) :
		_unixMilliseconds(unixMilliseconds)
	{
	}

	DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
	{
		int days = DaysSinceEpoch(year, month, day);

		_unixMilliseconds = days * MSecsPerDay +
			hour * MSecsPerHour +
			minute * MSecsPerMinute +
			second * MSecsPerSecond +
			millisecond;
	}

	bool DateTime::IsLeapYear(int year)
	{
		return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
	}

	int DateTime::GetYear() const
	{
		int year;
		int month;
		int day;

		EpochToYearsMonthDays(_unixMilliseconds, &year, &month, &day);

		return year;
	}

	int DateTime::GetMonth() const
	{
		int year;
		int month;
		int day;

		EpochToYearsMonthDays(_unixMilliseconds, &year, &month, &day);

		return month;
	}

	int DateTime::GetDay() const
	{
		int year;
		int month;
		int day;

		EpochToYearsMonthDays(_unixMilliseconds, &year, &month, &day);

		return day;
	}

	int DateTime::GetHour() const
	{
		return static_cast<int>(_unixMilliseconds / MSecsPerHour % 24);
	}

	int DateTime::GetMinute() const
	{
		return static_cast<int>(_unixMilliseconds / MSecsPerMinute % 60);
	}

	int DateTime::GetSecond() const
	{
		return static_cast<int>(_unixMilliseconds / MSecsPerSecond % 60);
	}

	int DateTime::GetMillisecond() const
	{
		return static_cast<int>(_unixMilliseconds % 1000);
	}

	double DateTime::GetTotalHours() const
	{
		return static_cast<double>(_unixMilliseconds) / MSecsPerHour;
	}

	double DateTime::GetTotalMinutes() const
	{
		return static_cast<double>(_unixMilliseconds) / MSecsPerMinute;
	}

	double DateTime::GetTotalSeconds() const
	{
		return static_cast<double>(_unixMilliseconds) / MSecsPerSecond;
	}

	TimeSpan DateTime::operator-(const DateTime& other) const
	{
		return TimeSpan(_unixMilliseconds * 1000 - other._unixMilliseconds * 1000);
	}

	int DateTime::DaysSinceEpoch(int year, int month, int day)
	{
		// Since the epoch is 0 indexed, we need to subtract by 1 day
		int dayCount = day - 1;

		int leapYearIndex = IsLeapYear(year) ? 1 : 0;

		// Add days in each month
		for (int m = 0; m < month - 1; m++)
		{
			dayCount += DaysPerMonth[leapYearIndex][m];
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

	void DateTime::EpochToYearsMonthDays(long long unixMilliseconds, int* year, int* month, int* day)
	{
		int daysSinceEpoch = static_cast<int>(unixMilliseconds / MSecsPerDay);
		int currentYear = UnixEpochYear;
		int currentDay = daysSinceEpoch;

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

		*year = currentYear;

		// Now figure out the month. Current day is the 0 indexed day of the year
		int leapYearIndex = IsLeapYear(currentYear) ? 1 : 0;
		int currentMonthIndex = 0;

		while (currentDay >= DaysPerMonth[leapYearIndex][currentMonthIndex])
		{
			currentDay -= DaysPerMonth[leapYearIndex][currentMonthIndex];
			currentMonthIndex++;
		}

		// Month and day are not 0 indexed, so add 1 to them
		*month = currentMonthIndex + 1;
		*day = currentDay + 1;
	}
}