#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/DateTime.h>
#include <Coco/Core/Types/TimeSpan.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(DateTimeTests)
	{
		TEST_METHOD(CreateMS)
		{
			constexpr int64 ms = 12379123;

			DateTime t(ms);
			Assert::AreEqual(t.UnixMilliseconds, ms);
		}

		TEST_METHOD(CreateFromDate)
		{
			constexpr int year = 2004;
			constexpr int month = 1;
			constexpr int day = 2;
			constexpr int hour = 5;
			constexpr int minute = 34;
			constexpr int second = 45;
			constexpr int ms = 753;

			DateTime t(year, month, day, hour, minute, second, ms);
			Assert::AreEqual(t.GetYear(), year);
			Assert::AreEqual(t.GetMonth(), month);
			Assert::AreEqual(t.GetDay(), day);
			Assert::AreEqual(t.GetHour(), hour);
			Assert::AreEqual(t.GetMinute(), minute);
			Assert::AreEqual(t.GetSecond(), second);
			Assert::AreEqual(t.GetMillisecond(), ms);
		}

		TEST_METHOD(DateOffset)
		{
			constexpr int year = 2012;
			constexpr int month = 3;
			constexpr int day = 22;
			constexpr int hour = 15;
			constexpr int minute = 2;
			constexpr int second = 23;
			constexpr int ms = 12;

			DateTime d(year, month, day, hour, minute, second, ms);

			d += TimeSpan::FromDays(1);
			Assert::AreEqual(d.GetDay(), day + 1);

			d += TimeSpan::FromHours(1);
			Assert::AreEqual(d.GetHour(), hour + 1);

			d += TimeSpan::FromMinutes(1);
			Assert::AreEqual(d.GetMinute(), minute + 1);

			d += TimeSpan::FromSeconds(1);
			Assert::AreEqual(d.GetSecond(), second + 1);

			d += TimeSpan::FromMilliseconds(1);
			Assert::AreEqual(d.GetMillisecond(), ms + 1);
		}
	};
}