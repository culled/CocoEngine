#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/TimeSpan.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(TimeSpanTests)
	{
		TEST_METHOD(CreateFromUSecs)
		{
			constexpr int64 value = 871293;

			TimeSpan s(value);
			Assert::IsTrue(s.Microseconds == value);
		}

		TEST_METHOD(CreateFromMilliseconds)
		{
			constexpr int64 value = 123312;

			TimeSpan s = TimeSpan::FromMilliseconds(value);
			Assert::IsTrue(Math::Equal(s.GetMilliseconds(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromSeconds)
		{
			constexpr int64 value = 12323;

			TimeSpan s = TimeSpan::FromSeconds(value);
			Assert::IsTrue(Math::Equal(s.GetSeconds(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromMinutes)
		{
			constexpr int64 value = 6732;

			TimeSpan s = TimeSpan::FromMinutes(value);
			Assert::IsTrue(Math::Equal(s.GetMinutes(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromHours)
		{
			constexpr int64 value = 23423;

			TimeSpan s = TimeSpan::FromHours(value);
			Assert::IsTrue(Math::Equal(s.GetHours(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromDays)
		{
			constexpr int64 value = 74553;

			TimeSpan s = TimeSpan::FromDays(value);
			Assert::IsTrue(Math::Equal(s.GetDays(), static_cast<double>(value)));
		}

		TEST_METHOD(TimespanOperations)
		{
			constexpr int64 v1 = 1234;
			constexpr int64 v2 = 864535;

			TimeSpan t1 = TimeSpan::FromMinutes(v1);
			t1 += TimeSpan::FromMinutes(v2);

			Assert::IsTrue(Math::Equal(t1.GetMinutes(), static_cast<double>(v1 + v2)));
		}
	};
}