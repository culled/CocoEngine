#pragma once

#include <Coco/Core/Core.h>

namespace Coco
{
	/// <summary>
	/// Represents a length of time
	/// </summary>
	struct COCOAPI TimeSpan
	{
	private:
		long long _microseconds;

	public:
		TimeSpan(long long microseconds = 0);

		/// <summary>
		/// Gets the number of days in this length of time
		/// </summary>
		/// <returns>The number of days</returns>
		int GetDays() const;

		/// <summary>
		/// Gets the number of hours in the day for this length of time
		/// </summary>
		/// <returns>The number of hours in the day</returns>
		int GetHours() const;

		/// <summary>
		/// Gets the number of minutes in the hour for this length of time
		/// </summary>
		/// <returns>The number of minutes in the hour</returns>
		int GetMinutes() const;

		/// <summary>
		/// Gets the number of seconds in the minute for this length of time
		/// </summary>
		/// <returns>The number of seconds in the minute</returns>
		int GetSeconds() const;

		/// <summary>
		/// Gets the number of milliseconds in the second for this length of time
		/// </summary>
		/// <returns>The number of milliseconds in the second</returns>
		int GetMilliseconds() const;

		/// <summary>
		/// Gets the number of microseconds in the millisecond for this length of time
		/// </summary>
		/// <returns>The number of microseconds in the millisecond</returns>
		int GetMicroseconds() const;

		/// <summary>
		/// Gets the fractional amount of days in this length of time
		/// </summary>
		/// <returns>The fractional amount of days</returns>
		double GetTotalDays() const;

		/// <summary>
		/// Gets the fractional amount of hours in this length of time
		/// </summary>
		/// <returns>The fractional amount of hours</returns>
		double GetTotalHours() const;

		/// <summary>
		/// Gets the fractional amount of minutes in this length of time
		/// </summary>
		/// <returns>The fractional amount of minutes</returns>
		double GetTotalMinutes() const;

		/// <summary>
		/// Gets the fractional amount of seconds in this length of time
		/// </summary>
		/// <returns>The fractional amount of seconds</returns>
		double GetTotalSeconds() const;

		/// <summary>
		/// Gets the fractional amount of milliseconds in this length of time
		/// </summary>
		/// <returns>The fractional amount of milliseconds</returns>
		double GetTotalMilliseconds() const;

		/// <summary>
		/// Gets the number of microseconds in this length of time
		/// </summary>
		/// <returns>The amount of microseconds</returns>
		long long GetTotalMicroseconds() const { return _microseconds; }
	};
}

