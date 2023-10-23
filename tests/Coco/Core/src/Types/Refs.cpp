#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Refs.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(ManagedRefTests)
	{
		struct A {
			int a;

			A(int a) : a(a) {}
		};

		struct B : A {
			int b;

			B(int a, int b) : A(a), b(b) {}
		};

		TEST_METHOD(CreateAndDestroy)
		{
			constexpr int value = 1;

			ManagedRef<int> t = CreateManagedRef<int>(value);
			Assert::IsTrue(t.IsValid());
			Assert::IsTrue(t.GetUseCount() == 1);
			Assert::AreEqual(*t, value);

			t.Invalidate();
			Assert::IsFalse(t.IsValid());
			Assert::IsTrue(t.GetUseCount() == 1);
		}

		TEST_METHOD(GetRef)
		{
			constexpr int value = 1;

			::Ref<int> r;
			Assert::IsFalse(r.IsValid());

			{
				ManagedRef<int> t = CreateManagedRef<int>(value);
				r = t;
				Assert::IsTrue(r.IsValid());
				Assert::AreEqual(*r, *t);
				Assert::AreEqual(r.Get(), t.Get());
				Assert::IsTrue(t.GetUseCount() == 2);

				{
					::Ref<int> r2 = r;
					Assert::IsTrue(r2.IsValid());
					Assert::AreEqual(*r2, *t);
					Assert::AreEqual(r2.Get(), t.Get());
					Assert::IsTrue(t.GetUseCount() == 3);
				}

				Assert::IsTrue(t.GetUseCount() == 2);
			}

			Assert::IsFalse(r.IsValid());
		}

		TEST_METHOD(Inheritance)
		{
			constexpr int a = 12;
			constexpr int b = 132;

			ManagedRef<B> m = CreateManagedRef<B>(a, b);
			Assert::AreEqual(m->a, a);
			Assert::AreEqual(m->b, b);

			::Ref<A> r = m;
			Assert::AreEqual(r->a, a);
		}
	};

	TEST_CLASS(RefTests)
	{
		TEST_METHOD(TemporaryRef)
		{
			constexpr int v = 123;
			UniqueRef<int> vRef = CreateUniqueRef<int>(v);

			Ref<int> tempRef;
			Assert::IsFalse(tempRef.IsValid());

			tempRef = Ref<int>(vRef.get());
			Assert::IsTrue(tempRef.IsValid());
			Assert::IsTrue(tempRef.GetUseCount() == 1);
			Assert::AreEqual(tempRef.Get(), vRef.get());

			tempRef.Invalidate();
			Assert::IsFalse(tempRef.IsValid());
		}

	};
}