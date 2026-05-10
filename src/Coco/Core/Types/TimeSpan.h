//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_TIMESPAN_H
#define COCOENGINE_TIMESPAN_H
#include <cmath>

#include "CoreTypes.h"
#include "Coco/Core/Math/Math.h"

namespace Coco
{
    /// @brief Represents a length of time with microsecond precision
    struct TimeSpan
    {
        /// @brief The number of microseconds in a millisecond
        static constexpr int64 MicroSecsPerMillisecond = 1000;

        /// @brief The number of microseconds in a second
        static constexpr int64 MicroSecsPerSecond = 1000000;

        /// @brief The number of microseconds in a minute
        static constexpr int64 MicroSecsPerMinute = 60000000;

        /// @brief The number of microseconds in an hour
        static constexpr int64 MicroSecsPerHour = 3600000000;

        /// @brief The number of microseconds in a day
        static constexpr int64 MicroSecsPerDay = 86400000000;

        /// @brief The number of microseconds
        int64 Microseconds = 0;

        TimeSpan() noexcept;
        TimeSpan(int64 microseconds) noexcept;

        void operator+=(const TimeSpan& rhs) noexcept { Microseconds += rhs.Microseconds; }
        void operator-=(const TimeSpan& rhs) noexcept { Microseconds -= rhs.Microseconds; }

        void operator*=(const TimeSpan& rhs) noexcept { Microseconds *= rhs.Microseconds; }
        void operator/=(const TimeSpan& rhs) noexcept { Microseconds /= rhs.Microseconds; }

        template<typename ScalarType>
        void operator*=(const ScalarType& scalar) noexcept
        {
            Microseconds = static_cast<uint64>(floor(static_cast<ScalarType>(Microseconds) * scalar));
        }

        template<typename ScalarType>
        void operator/=(const ScalarType& scalar) noexcept
        {
            Microseconds = static_cast<uint64>(floor(static_cast<ScalarType>(Microseconds) / scalar));
        }

        /// @brief Creates a TimeSpan with the given number of fractional days
        /// @param days The number of fractional days
        /// @return A TimeSpan
        static TimeSpan FromDays(double days) noexcept
        {
            return {static_cast<int64>(floor(days * MicroSecsPerDay))};
        }

        /// @brief Creates a TimeSpan with the given number of fractional hours
        /// @param hours The number of fractional hours
        /// @return A TimeSpan
        static TimeSpan FromHours(double hours) noexcept
        {
            return {static_cast<int64>(floor(hours * MicroSecsPerHour))};
        }

        /// @brief Creates a TimeSpan with the given number of fractional minutes
        /// @param minutes The number of fractional minutes
        /// @return A TimeSpan
        static TimeSpan FromMinutes(double minutes) noexcept
        {
            return {static_cast<int64>(floor(minutes * MicroSecsPerMinute))};
        }

        /// @brief Creates a TimeSpan with the given number of fractional seconds
        /// @param seconds The number of fractional seconds
        /// @return A TimeSpan
        static TimeSpan FromSeconds(double seconds) noexcept
        {
            return {static_cast<int64>(floor(seconds * MicroSecsPerSecond))};
        }

        /// @brief Creates a TimeSpan with the given number of fractional milliseconds
        /// @param milliseconds The number of fractional milliseconds
        /// @return A TimeSpan
        static TimeSpan FromMilliseconds(double milliseconds) noexcept
        {
            return {static_cast<int64>(floor(milliseconds * MicroSecsPerMillisecond))};
        }

        /// @brief Gets the number of days in this TimeSpan
        /// @return The number of days
        constexpr double GetDays() const noexcept { return static_cast<double>(Microseconds) / MicroSecsPerDay; }

        /// @brief Gets the number of hours in this TimeSpan
        /// @return The number of hours
        constexpr double GetHours() const noexcept { return static_cast<double>(Microseconds) / MicroSecsPerHour; }

        /// @brief Gets the number of minutes in this TimeSpan
        /// @return The number of minutes
        constexpr double GetMinutes() const noexcept { return static_cast<double>(Microseconds) / MicroSecsPerMinute; }

        /// @brief Gets the number of seconds in this TimeSpan
        /// @return The number of seconds
        constexpr double GetSeconds() const noexcept { return static_cast<double>(Microseconds) / MicroSecsPerSecond; }

        /// @brief Gets the number of milliseconds in this TimeSpan
        /// @return The number of milliseconds
        constexpr double GetMilliseconds() const noexcept
        {
            return static_cast<double>(Microseconds) / MicroSecsPerMillisecond;
        }
    };

    constexpr auto operator<=>(const TimeSpan& a, const TimeSpan& b) noexcept { return a.Microseconds <=> b.Microseconds; }

    inline TimeSpan operator+(const TimeSpan& a, const TimeSpan& b) noexcept { return {a.Microseconds + b.Microseconds}; }
    inline TimeSpan operator-(const TimeSpan& a, const TimeSpan& b) noexcept { return {a.Microseconds - b.Microseconds}; }

    inline TimeSpan operator*(const TimeSpan& a, const TimeSpan& b) noexcept { return {a.Microseconds * b.Microseconds}; }
    inline TimeSpan operator/(const TimeSpan& a, const TimeSpan& b) noexcept { return {a.Microseconds / b.Microseconds}; }

    template<typename ScalarType>
    TimeSpan operator*(const TimeSpan& a, const ScalarType& scalar)  noexcept
    {
        return TimeSpan(static_cast<uint64>(floor(static_cast<ScalarType>(a.Microseconds) * scalar)));
    }

    template<typename ScalarType>
    TimeSpan operator/(const TimeSpan& a, const ScalarType& scalar)  noexcept
    {
        return TimeSpan(static_cast<uint64>(floor(static_cast<ScalarType>(a.Microseconds) / scalar)));
    }

    template<>
    const TimeSpan& Math::Max(const TimeSpan& a, const TimeSpan& b) noexcept;

    template<>
    const TimeSpan& Math::Min(const TimeSpan& a, const TimeSpan& b) noexcept;
} // Coco

#endif //COCOENGINE_TIMESPAN_H
