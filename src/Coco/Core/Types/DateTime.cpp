//
// Created by cullen on 2/24/26.
//

#include "DateTime.h"

#include <algorithm>
#include <cmath>

#include "TimeSpan.h"
#include "StackArray.h"
#include "Coco/Core/Asserts.h"
#include "../Engine.h"

namespace Coco
{
    /// @brief The days in each month. Index 0 is a non-leap year, index 1 is a leap year
	StackArray<StackArray<uint8, 12>, 2> DaysPerMonth = {
		StackArray<uint8, 12>({ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }),
		StackArray<uint8, 12>({ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 })
	};

	DateTime::DateTime() noexcept :
		DateTime(0)
	{}

	DateTime::DateTime(int64 unixMilliseconds) noexcept :
		UnixMilliseconds(unixMilliseconds)
	{}

	DateTime::DateTime(int year, uint8 month, uint8 day, uint8 hour, uint8 minute, uint8 second, uint16 millisecond) noexcept :
		DateTime(TimeToUnixMilliseconds(year, month, day, hour, minute, second, millisecond))
	{}

	void DateTime::operator+=(const TimeSpan& time) noexcept
	{
		UnixMilliseconds += static_cast<int64>(floor(time.GetMilliseconds()));
	}

	void DateTime::operator-=(const TimeSpan& time) noexcept
	{
		UnixMilliseconds -= static_cast<int64>(floor(time.GetMilliseconds()));
	}

	int DateTime::DaysSinceEpoch(int year, uint8 month, uint8 day) noexcept
	{
		if (day < 1 || day > 31)
			day = Math::Clamp(month, static_cast<uint8>(1), static_cast<uint8>(31));

		if (month < 1 || month > 12)
			month = Math::Clamp(month, static_cast<uint8>(1), static_cast<uint8>(12));

		// Since the epoch is 0 indexed, we need to subtract by 1 day
		int dayCount = static_cast<int>(day) - 1;

		const uint8 leapYearIndex = IsLeapYear(year) ? 1 : 0;

		// Add days in each month
		for (uint8 m = 0; m < month - 1; m++)
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

		// Include the current day in the result
		return dayCount + 1;
	}

	void DateTime::EpochToYearsMonthDays(int64 unixMilliseconds, int& outYear, uint8& outMonth, uint8& outDay) noexcept
	{
		const int daysSinceEpoch = static_cast<int>(unixMilliseconds / MSecsPerDay);
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

		outYear = currentYear;

		// Now figure out the month. Current day is the 0 indexed day of the year
		const int leapYearIndex = IsLeapYear(currentYear) ? 1 : 0;
		uint8 currentMonthIndex = 0;

		while (currentDay >= DaysPerMonth[leapYearIndex][currentMonthIndex])
		{
			currentDay -= DaysPerMonth[leapYearIndex][currentMonthIndex];
			currentMonthIndex++;
		}

		// Month is not 0 indexed, so add 1 to it
		outMonth = currentMonthIndex + 1;

		COCO_ASSERT(currentDay > 0 && currentDay <= 31, "Current day did not fall in the range 1 - 31");
		outDay = static_cast<uint8>(currentDay);
	}

	int64 DateTime::TimeToUnixMilliseconds(int year, uint8 month, uint8 day, uint8 hour, uint8 minute, uint8 second, uint16 millisecond) noexcept
	{
		int days = DaysSinceEpoch(year, month, day);

		return days * MSecsPerDay + hour * MSecsPerHour + minute * MSecsPerMinute + second * MSecsPerSecond + millisecond;
	}

	DateTime DateTime::Now()
	{
		return Engine::Get()->GetPlatform()->GetLocalTime();
	}

	DateTime DateTime::NowUtc()
	{
		return Engine::Get()->GetPlatform()->GetUtcTime();
	}

	int DateTime::GetYear() const noexcept
	{
		int year;
		uint8 month, day;
		EpochToYearsMonthDays(UnixMilliseconds, year, month, day);

		return year;
	}

	uint8 DateTime::GetMonth() const noexcept
	{
		int year;
		uint8 month, day;
		EpochToYearsMonthDays(UnixMilliseconds, year, month, day);

		return month;
	}

	uint8 DateTime::GetDay() const noexcept
	{
		int year;
		uint8 month, day;
		EpochToYearsMonthDays(UnixMilliseconds, year, month, day);

		return day;
	}

	TimeSpan DateTime::GetTimeSinceEpoch() const noexcept
	{
		return TimeSpan::FromMilliseconds(static_cast<double>(UnixMilliseconds));
	}

	DateTime operator+(const DateTime& dateTime, const TimeSpan& time) noexcept
	{
		return {dateTime.UnixMilliseconds + static_cast<int64>(floor(time.GetMilliseconds()))};
	}

	DateTime operator-(const DateTime& dateTime, const TimeSpan& time) noexcept
	{
		return {dateTime.UnixMilliseconds - static_cast<int64>(floor(time.GetMilliseconds()))};
	}

}
