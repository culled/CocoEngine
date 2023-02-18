#include "TimeSpan.h"

namespace Coco
{
	const long long MicroSecsPerMillisecond = 1000;
	const long long MicroSecsPerSecond = 1000000;
	const long long MicroSecsPerMinute = 60000000;
	const long long MicroSecsPerHour = 3600000000;
	const long long MicroSecsPerDay = 86400000000;

	TimeSpan::TimeSpan(long long microseconds) :
		_microseconds(microseconds)
	{}

	TimeSpan TimeSpan::FromDays(double days)
	{
		return TimeSpan(static_cast<long long>(days * MicroSecsPerDay));
	}

	TimeSpan TimeSpan::FromHours(double hours)
	{
		return TimeSpan(static_cast<long long>(hours * MicroSecsPerHour));
	}

	TimeSpan TimeSpan::FromMinutes(double minutes)
	{
		return TimeSpan(static_cast<long long>(minutes * MicroSecsPerMinute));
	}

	TimeSpan TimeSpan::FromSeconds(double seconds)
	{
		return TimeSpan(static_cast<long long>(seconds * MicroSecsPerSecond));
	}

	TimeSpan TimeSpan::FromMilliseconds(double milliseconds)
	{
		return TimeSpan(static_cast<long long>(milliseconds * MicroSecsPerMillisecond));
	}

	int TimeSpan::GetDays() const
	{
		return static_cast<int>(_microseconds / MicroSecsPerDay);
	}

	int TimeSpan::GetHours() const
	{
		return static_cast<int>(_microseconds / MicroSecsPerHour % 24);
	}

	int TimeSpan::GetMinutes() const
	{
		return static_cast<int>(_microseconds / MicroSecsPerMinute % 60);
	}

	int TimeSpan::GetSeconds() const
	{
		return static_cast<int>(_microseconds / MicroSecsPerSecond % 60);
	}

	int TimeSpan::GetMilliseconds() const
	{
		return static_cast<int>(_microseconds / MicroSecsPerMillisecond % 1000);
	}

	int TimeSpan::GetMicroseconds() const
	{
		return static_cast<int>(_microseconds % MicroSecsPerMillisecond);
	}

	double TimeSpan::GetTotalDays() const
	{
		return static_cast<double>(_microseconds) / MicroSecsPerDay;
	}

	double TimeSpan::GetTotalHours() const
	{
		return static_cast<double>(_microseconds) / MicroSecsPerHour;
	}

	double TimeSpan::GetTotalMinutes() const
	{
		return static_cast<double>(_microseconds) / MicroSecsPerMinute;
	}

	double TimeSpan::GetTotalSeconds() const
	{
		return static_cast<double>(_microseconds) / MicroSecsPerSecond;
	}

	double TimeSpan::GetTotalMilliseconds() const
	{
		return static_cast<double>(_microseconds) / MicroSecsPerMillisecond;
	}

	TimeSpan TimeSpan::operator+(const TimeSpan& other) const
	{
		return TimeSpan(this->_microseconds + other._microseconds);
	}

	TimeSpan TimeSpan::operator-(const TimeSpan& other) const
	{
		return TimeSpan(this->_microseconds - other._microseconds);
	}

	void TimeSpan::operator+=(const TimeSpan& other)
	{
		_microseconds += other._microseconds;
	}

	void TimeSpan::operator-=(const TimeSpan& other)
	{
		_microseconds -= other._microseconds;
	}

	bool TimeSpan::operator<(const TimeSpan& other)
	{
		return _microseconds < other._microseconds;
	}

	bool TimeSpan::operator<=(const TimeSpan& other)
	{
		return _microseconds <= other._microseconds;
	}

	bool TimeSpan::operator>(const TimeSpan& other)
	{
		return _microseconds > other._microseconds;
	}

	bool TimeSpan::operator>=(const TimeSpan& other)
	{
		return _microseconds >= other._microseconds;
	}

	bool TimeSpan::operator==(const TimeSpan& other)
	{
		return _microseconds == other._microseconds;
	}

	bool TimeSpan::operator!=(const TimeSpan& other)
	{
		return *this != other;
	}
}
