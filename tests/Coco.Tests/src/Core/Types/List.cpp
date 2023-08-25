#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/List.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(ListTests)
	{
		TEST_METHOD(AddAndRemove)
		{
			constexpr int value = 3;

			List<int> l;
			Assert::IsTrue(l.Count() == 0);

			l.Add(value);
			Assert::IsTrue(l.Count() == 1);
			Assert::AreEqual(l.First(), value);

			l.Remove(value);
			Assert::IsTrue(l.Count() == 0);
		}

		TEST_METHOD(InsertAndRemove)
		{
			constexpr int value = 3;
			constexpr int value2 = 3;

			List<int> l;
			l.Add(value);
			Assert::IsTrue(l.Count() == 1);
			Assert::AreEqual(l.First(), value);

			l.Insert(0, value2);
			Assert::IsTrue(l.Count() == 2);
			Assert::AreEqual(l.First(), value2);
			Assert::AreEqual(l[1], value);

			l.Remove(value);
			Assert::IsTrue(l.Count() == 1);
			Assert::AreEqual(l.First(), value2);
		}

		TEST_METHOD(Find)
		{
			List<int> l;
			l.Add(1);
			l.Add(2);
			l.Add(3);
			l.Add(4);
			l.Add(2);

			auto it = l.Find(3);
			Assert::IsTrue(std::distance(l.begin(), it) == 2);

			it = l.Find([](const auto& v) { return v == 2; });
			Assert::IsTrue(std::distance(l.begin(), it) == 1);

			Assert::IsTrue(l.Contains(4));
			Assert::IsFalse(l.Contains(6));
			Assert::IsTrue(l.Any([](const auto& v) { return v == 1; }));
			Assert::IsFalse(l.All([](const auto& v) { return v == 2; }));
		}

		TEST_METHOD(Resizing)
		{
			List<int> l;
			l.Add(1);
			l.Add(2);
			l.Add(3);
			l.Add(4);

			Assert::IsTrue(l.Count() == 4);

			l.Resize(2);
			Assert::IsTrue(l.Count() == 2);
		}
	};
}