//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_DATETIME_H
#define COCOENGINE_DATETIME_H
#include <compare>

#include "CoreTypes.h"

namespace Coco
{
    struct TimeSpan;

    /// @brief Represents an absolute point in time, with millisecond precision
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

        DateTime() noexcept;
        DateTime(int64 unixMilliseconds) noexcept;
        DateTime(int year, uint8 month, uint8 day, uint8 hour, uint8 minute, uint8 second, uint16 millisecond) noexcept;

        void operator+=(const TimeSpan& time) noexcept;
        void operator-=(const TimeSpan& time) noexcept;

        /// @brief Gets if the given year is a leap year
        /// @param year The year
        /// @return True if the year is a leap year
        static constexpr bool IsLeapYear(int year) noexcept
        {
            return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
        }

        /// @brief Calculates the total amount of days since unix epoch for a date
        /// @param year The year
        /// @param month The month
        /// @param day The day
        /// @return The number of days since epoch the given date is
        static int DaysSinceEpoch(int year, uint8 month, uint8 day) noexcept;

        /// @brief Calculates the date given the milliseconds since unix epoch
        /// @param unixMilliseconds The milliseconds since unix epoch
        /// @param outYear Will be filled with the year
        /// @param outMonth Will be filled with the month
        /// @param outDay Will be filled with the day
        static void EpochToYearsMonthDays(int64 unixMilliseconds, int& outYear, uint8& outMonth,
                                          uint8& outDay) noexcept;

        /// @brief Calculates the number of milliseconds from epoch a date is
        /// @param year The year
        /// @param month The month
        /// @param day The day
        /// @param hour The hour
        /// @param minute The minute
        /// @param second The second
        /// @param millisecond The millisecond
        /// @return The milliseconds since unix epoch
        static int64 TimeToUnixMilliseconds(int year, uint8 month, uint8 day, uint8 hour, uint8 minute, uint8 second,
                                            uint16 millisecond) noexcept;

        /// @brief Gets the current local time of the system
        /// @return The current local time
        static DateTime Now();

        /// @brief Gets the current UTC time of the system
        /// @return The current UTC time
        static DateTime NowUtc();

        /// @brief Gets the year this DateTime represents
        /// @return The year
        int GetYear() const noexcept;

        /// @brief Gets the month this DateTime represents
        /// @return The month
        uint8 GetMonth() const noexcept;

        /// @brief Gets the day this DateTime represents
        /// @return The day
        uint8 GetDay() const noexcept;

        /// @brief Gets the hour this DateTime represents
        /// @return The hour
        constexpr uint8 GetHour() const noexcept { return static_cast<uint8>((UnixMilliseconds / MSecsPerHour) % 24); }

        /// @brief Gets the minute this DateTime represents
        /// @return The minute
        constexpr uint8 GetMinute() const noexcept
        {
            return static_cast<uint8>((UnixMilliseconds / MSecsPerMinute) % 60);
        }

        /// @brief Gets the second this DateTime represents
        /// @return The second
        constexpr uint8 GetSecond() const noexcept
        {
            return static_cast<uint8>((UnixMilliseconds / MSecsPerSecond) % 60);
        }

        /// @brief Gets the millisecond this DateTime represents
        /// @return The millisecond
        constexpr uint16 GetMillisecond() const noexcept
        {
            return static_cast<uint16>(UnixMilliseconds % MSecsPerSecond);
        }

        /// @brief Gets the fractional hours since unix epoch
        /// @return The fractional hours
        constexpr double GetTotalHours() const noexcept { return static_cast<double>(UnixMilliseconds) / MSecsPerHour; }

        /// @brief Gets the fractional minutes since unix epoch
        /// @return The fractional minutes
        constexpr double GetTotalMinutes() const noexcept
        {
            return static_cast<double>(UnixMilliseconds) / MSecsPerMinute;
        }

        /// @brief Gets the fractional seconds since unix epoch
        /// @return The fractional seconds
        constexpr double GetTotalSeconds() const noexcept
        {
            return static_cast<double>(UnixMilliseconds) / MSecsPerSecond;
        }

        /// @brief Gets the time since unix epoch
        /// @return The time since epoch
        TimeSpan GetTimeSinceEpoch() const noexcept;
    };

    constexpr auto operator<=>(const DateTime& a, const DateTime& b) noexcept
    {
        return a.UnixMilliseconds <=> b.UnixMilliseconds;
    }

    DateTime operator+(const DateTime& dateTime, const TimeSpan& time) noexcept;
    DateTime operator-(const DateTime& dateTime, const TimeSpan& time) noexcept;
}

#endif //COCOENGINE_DATETIME_H
