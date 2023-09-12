#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Events/Query.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Events
{
	TEST_CLASS(QueryTests)
	{
		TEST_METHOD(AddRemoveQueryHandler)
		{
			// Create a query and make sure it has no handlers
			Query<int> e;
			Assert::IsTrue(e.GetHandlerCount() == 0);

			// Create a query handler and make sure it is disconnected
			QueryHandler<int> h;
			Assert::IsFalse(h.IsConnected());

			// Connect the handler to the query
			h.Connect(&e);
			Assert::IsTrue(e.GetHandlerCount() == 1);
			Assert::IsTrue(h.IsConnected());

			// Disconnect the handler from the query
			h.Disconnect();
			Assert::IsFalse(h.IsConnected());
			Assert::IsTrue(e.GetHandlerCount() == 0);
		}

		TEST_METHOD(AddQueryHandlerAndInvoke)
		{
			Query<int, int> e;
			constexpr int testValue = 1231;

			int v = 0;
			// Make sure the query is not handled
			Assert::IsFalse(e.Invoke(v, testValue));

			// Connect a query handler with a callback function
			QueryHandler<int, int> h([testValue](int& v, int a) { Assert::AreEqual(a, testValue); v = a; return true; });
			h.Connect(&e);

			// Invoke the query, make sure it is handled, and make sure the query value gets modified correctly
			Assert::IsTrue(e.Invoke(v, testValue));
			Assert::AreEqual(v, testValue);

			// Disconnect the handler and make sure the query returns to its unhandled state
			h.Disconnect();
			Assert::IsFalse(e.Invoke(v, testValue));
		}

		TEST_METHOD(QueryHandledBeforeOtherHandler)
		{
			Query<int> e;

			// Create a handler that will fail the test if called and connect it
			QueryHandler<int> h1([](int& v) { Assert::Fail(); return true; });
			h1.Connect(&e);

			// Connect another handler which should handle the query and prevent the failing handler from being called
			QueryHandler<int> h2([](int& v) { return true; });
			h2.Connect(&e);

			// Invoke the query
			int v = 0;
			Assert::IsTrue(e.Invoke(v));
		}

		TEST_METHOD(AutoDisconnectQueryHandler)
		{
			Query<int> e;

			{
				// Create a handler in this scope and connect it
				QueryHandler<int> h;
				h.Connect(&e);

				Assert::IsTrue(e.GetHandlerCount() == 1);
			}

			// The handler should auto-disconnect when it gets destroyed (goes out of scope)
			Assert::IsTrue(e.GetHandlerCount() == 0);
		}

		TEST_METHOD(AutoDisconnectQuery)
		{
			QueryHandler<int> h;

			{
				// Create a query in this scope and connect the handler
				Query<int> e;
				h.Connect(&e);

				Assert::IsTrue(h.IsConnected());
			}

			// The handler should auto-disconnect when the event gets destroyed (goes out of scope)
			Assert::IsFalse(h.IsConnected());
		}
	};
}