#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Events/Event.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Events
{
	TEST_CLASS(EventTests)
	{
		TEST_METHOD(AddRemoveEventHandler)
		{
			// Create an event and make sure it has no handlers
			Event<> e;
			Assert::IsTrue(e.GetHandlerCount() == 0);

			// Create an event handler and make sure it is disconnected
			EventHandler<> h;
			Assert::IsFalse(h.IsConnected());

			// Connect the handler to the event
			h.Connect(e);
			Assert::IsTrue(e.GetHandlerCount() == 1);
			Assert::IsTrue(h.IsConnected());

			// Disconnect the handler from the event
			h.Disconnect();
			Assert::IsFalse(h.IsConnected());
			Assert::IsTrue(e.GetHandlerCount() == 0);
		}

		TEST_METHOD(AddEventHandlerAndInvoke)
		{
			Event<int> e;
			constexpr int testValue = 1231;

			// Make sure the event is not handled
			Assert::IsFalse(e.Invoke(testValue));

			// Connect an event handler with a callback function
			EventHandler<int> h([testValue](int v) { Assert::AreEqual(v, testValue); return true; });
			h.Connect(e);

			// Invoke the event and make sure it is handled
			Assert::IsTrue(e.Invoke(testValue));

			// Disconnect the handler and make sure the event returns to its unhandled state
			h.Disconnect();
			Assert::IsFalse(e.Invoke(testValue));
		}

		TEST_METHOD(EventHandledBeforeOtherHandler)
		{
			Event<> e;

			// Create a handler that will fail the test if called and connect it
			EventHandler<> h1([]() { Assert::Fail(); return true; });
			h1.Connect(e);

			// Connect another handler which should handle the event and prevent the failing handler from being called
			EventHandler<> h2([]() { return true; });
			h2.Connect(e);

			// Invoke the event
			Assert::IsTrue(e.Invoke());
		}

		TEST_METHOD(AutoDisconnectEventHandler)
		{
			Event<> e;

			{
				// Create a handler in this scope and connect it
				EventHandler<> h;
				h.Connect(e);

				Assert::IsTrue(e.GetHandlerCount() == 1);
			}

			// The handler should auto-disconnect when it gets destroyed (goes out of scope)
			Assert::IsTrue(e.GetHandlerCount() == 0);
		}

		TEST_METHOD(AutoDisconnectEvent)
		{
			EventHandler<> h;

			{
				// Create an event in this scope and connect the handler
				Event<> e;
				h.Connect(e);

				Assert::IsTrue(h.IsConnected());
			}

			// The handler should auto-disconnect when the event gets destroyed (goes out of scope)
			Assert::IsFalse(h.IsConnected());
		}
	};
}