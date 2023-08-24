#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Events/Query.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Events
{
	TEST_CLASS(QueryTests)
	{
		TEST_METHOD(AddRemoveQueryHandler)
		{
			Query<int> e;
			Assert::AreEqual(e.GetHandlerCount(), static_cast<uint64_t>(0));

			auto handler = e.AddHandler([]() { return 0; });
			Assert::AreEqual(e.GetHandlerCount(), static_cast<uint64_t>(1));

			e.RemoveHandler(handler);
			Assert::AreEqual(e.GetHandlerCount(), static_cast<uint64_t>(0));
		}

		TEST_METHOD(AddQueryHandlerAndCallQuery)
		{
			Query<int, int> e;
			constexpr int testValue = 1231;
			constexpr int testReturnValue = 2453;

			int v = 0;
			Assert::IsFalse(e.Invoke(&v, testValue));
			Assert::AreEqual(v, 0);

			auto handler = e.AddHandler([](int v) { Assert::AreEqual(v, testValue); return testReturnValue; });

			Assert::IsTrue(e.Invoke(&v, testValue));
			Assert::AreEqual(v, testReturnValue);

			e.RemoveHandler(handler);

			v = 0;
			Assert::IsFalse(e.Invoke(&v, testValue));
			Assert::AreEqual(v, 0);
		}

		TEST_METHOD(QueryHandledBeforeOtherHandler)
		{
			Query<int> e;
			constexpr int testReturnValue = 12;

			auto handler1 = e.AddHandler([]() { Assert::Fail(); return 0; });
			auto handler2 = e.AddHandler([]() { return testReturnValue; });

			int v = 0;
			Assert::IsTrue(e.Invoke(&v));
			Assert::AreEqual(v, testReturnValue);
		}
	};
}