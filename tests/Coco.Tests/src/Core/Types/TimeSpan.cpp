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
			constexpr int64_t value = 871293;
			
			TimeSpan s(value);
			Assert::IsTrue(Math::Approximately(s.GetTotalMicroseconds(), value));
		}

		TEST_METHOD(CreateFromMilliseconds)
		{
			constexpr int64_t value = 123312;

			TimeSpan s = TimeSpan::FromMilliseconds(value);
			Assert::IsTrue(Math::Approximately(s.GetTotalMilliseconds(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromSeconds)
		{
			constexpr int64_t value = 12323;

			TimeSpan s = TimeSpan::FromSeconds(value);
			Assert::IsTrue(Math::Approximately(s.GetTotalSeconds(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromMinutes)
		{
			constexpr int64_t value = 6732;

			TimeSpan s = TimeSpan::FromMinutes(value);
			Assert::IsTrue(Math::Approximately(s.GetTotalMinutes(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromHours)
		{
			constexpr int64_t value = 23423;

			TimeSpan s = TimeSpan::FromHours(value);
			Assert::IsTrue(Math::Approximately(s.GetTotalHours(), static_cast<double>(value)));
		}

		TEST_METHOD(CreateFromDays)
		{
			constexpr int64_t value = 74553;

			TimeSpan s = TimeSpan::FromDays(value);
			Assert::IsTrue(Math::Approximately(s.GetTotalDays(), static_cast<double>(value)));
		}

		TEST_METHOD(TimespanOperations)
		{
			constexpr int64_t v1 = 1234;
			constexpr int64_t v2 = 864535;

			TimeSpan t1 = TimeSpan::FromMinutes(v1);
			t1 += TimeSpan::FromMinutes(v2);

			Assert::IsTrue(Math::Approximately(t1.GetTotalMinutes(), static_cast<double>(v1 + v2)));
		}
	};
}