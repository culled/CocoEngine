#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Events/Event.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace CocoTests
{
	TEST_CLASS(EventTests)
	{
		TEST_METHOD(AddRemoveEventHandler)
		{
			Event<> e;
			Assert::AreEqual(e.GetHandlerCount(), static_cast<uint64_t>(0));

			auto handler = e.AddHandler([]() { return false; });
			Assert::AreEqual(e.GetHandlerCount(), static_cast<uint64_t>(1));

			e.RemoveHandler(handler);
			Assert::AreEqual(e.GetHandlerCount(), static_cast<uint64_t>(0));
		}

		TEST_METHOD(AddEventHandlerAndCallEvent)
		{
			Event<int> e;
			constexpr int testValue = 1231;

			Assert::IsFalse(e.Invoke(testValue));

			auto handler = e.AddHandler([](int v) { Assert::AreEqual(v, testValue); return true; });

			Assert::IsTrue(e.Invoke(testValue));

			e.RemoveHandler(handler);

			Assert::IsFalse(e.Invoke(testValue));
		}

		TEST_METHOD(EventHandledBeforeOtherHandler)
		{
			Event<> e;
			auto handler1 = e.AddHandler([]() { Assert::Fail(); return false; });
			auto handler2 = e.AddHandler([]() { return true; });

			Assert::IsTrue(e.Invoke());
		}
	};
}