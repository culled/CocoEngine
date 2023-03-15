#include "TimeSpan.h"

namespace Coco
{
	constexpr int64_t MicroSecsPerMillisecond = 1000;
	constexpr int64_t MicroSecsPerSecond = 1000000;
	constexpr int64_t MicroSecsPerMinute = 60000000;
	constexpr int64_t MicroSecsPerHour = 3600000000;
	constexpr int64_t MicroSecsPerDay = 86400000000;

	TimeSpan::TimeSpan(int64_t microseconds) noexcept :
		_microseconds(microseconds)
	{}

	TimeSpan TimeSpan::FromDays(double days) noexcept
	{
		return TimeSpan(static_cast<int64_t>(days) * MicroSecsPerDay);
	}

	TimeSpan TimeSpan::FromHours(double hours) noexcept
	{
		return TimeSpan(static_cast<int64_t>(hours) * MicroSecsPerHour);
	}

	TimeSpan TimeSpan::FromMinutes(double minutes) noexcept
	{
		return TimeSpan(static_cast<int64_t>(minutes) * MicroSecsPerMinute);
	}

	TimeSpan TimeSpan::FromSeconds(double seconds) noexcept
	{
		return TimeSpan(static_cast<int64_t>(seconds) * MicroSecsPerSecond);
	}

	TimeSpan TimeSpan::FromMilliseconds(double milliseconds) noexcept
	{
		return TimeSpan(static_cast<int64_t>(milliseconds) * MicroSecsPerMillisecond);
	}

	int TimeSpan::GetDays() const noexcept
	{
		return static_cast<int>(_microseconds / MicroSecsPerDay);
	}

	int TimeSpan::GetHours() const noexcept
	{
		return static_cast<int>(_microseconds / MicroSecsPerHour % 24);
	}

	int TimeSpan::GetMinutes() const noexcept
	{
		return static_cast<int>(_microseconds / MicroSecsPerMinute % 60);
	}

	int TimeSpan::GetSeconds() const noexcept
	{
		return static_cast<int>(_microseconds / MicroSecsPerSecond % 60);
	}

	int TimeSpan::GetMilliseconds() const noexcept
	{
		return static_cast<int>(_microseconds / MicroSecsPerMillisecond % 1000);
	}

	int TimeSpan::GetMicroseconds() const noexcept
	{
		return static_cast<int>(_microseconds % MicroSecsPerMillisecond);
	}

	double TimeSpan::GetTotalDays() const noexcept
	{
		return static_cast<double>(_microseconds) / MicroSecsPerDay;
	}

	double TimeSpan::GetTotalHours() const noexcept
	{
		return static_cast<double>(_microseconds) / MicroSecsPerHour;
	}

	double TimeSpan::GetTotalMinutes() const noexcept
	{
		return static_cast<double>(_microseconds) / MicroSecsPerMinute;
	}

	double TimeSpan::GetTotalSeconds() const noexcept
	{
		return static_cast<double>(_microseconds) / MicroSecsPerSecond;
	}

	double TimeSpan::GetTotalMilliseconds() const noexcept
	{
		return static_cast<double>(_microseconds) / MicroSecsPerMillisecond;
	}

	TimeSpan TimeSpan::operator+(const TimeSpan& other) const noexcept
	{
		return TimeSpan(this->_microseconds + other._microseconds);
	}

	TimeSpan TimeSpan::operator-(const TimeSpan& other) const noexcept
	{
		return TimeSpan(this->_microseconds - other._microseconds);
	}

	void TimeSpan::operator+=(const TimeSpan& other) noexcept
	{
		_microseconds += other._microseconds;
	}

	void TimeSpan::operator-=(const TimeSpan& other) noexcept
	{
		_microseconds -= other._microseconds;
	}

	bool TimeSpan::operator<(const TimeSpan& other) noexcept
	{
		return _microseconds < other._microseconds;
	}

	bool TimeSpan::operator<=(const TimeSpan& other) noexcept
	{
		return _microseconds <= other._microseconds;
	}

	bool TimeSpan::operator>(const TimeSpan& other) noexcept
	{
		return _microseconds > other._microseconds;
	}

	bool TimeSpan::operator>=(const TimeSpan& other) noexcept
	{
		return _microseconds >= other._microseconds;
	}

	bool TimeSpan::operator==(const TimeSpan& other) noexcept
	{
		return _microseconds == other._microseconds;
	}

	bool TimeSpan::operator!=(const TimeSpan& other) noexcept
	{
		return !(*this == other);
	}
}
