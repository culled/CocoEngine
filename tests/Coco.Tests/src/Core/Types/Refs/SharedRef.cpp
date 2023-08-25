#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Refs/SharedRef.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types::Ref
{
	TEST_CLASS(SharedRefTests)
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
			constexpr int value = 7;

			SharedRef<int> r = CreateSharedRef<int>(value);
			Assert::IsTrue(r.IsValid());
			Assert::AreEqual(*r, value);
			Assert::AreEqual(r.GetUseCount(), static_cast<uint64_t>(1));

			r.Reset();
			Assert::IsFalse(r.IsValid());
			Assert::AreEqual(r.GetUseCount(), static_cast<uint64_t>(1));
		}

		TEST_METHOD(MultipleSharedRefs)
		{
			constexpr int value = 10;

			SharedRef<int> v1 = CreateSharedRef<int>(value);
			Assert::AreEqual(v1.GetUseCount(), static_cast<uint64_t>(1));

			{
				SharedRef<int> v2 = v1;
				Assert::AreEqual(v1.GetUseCount(), static_cast<uint64_t>(2));
				Assert::AreEqual(v2.GetUseCount(), static_cast<uint64_t>(2));
				Assert::AreEqual(v1.Get(), v2.Get());
			}

			Assert::AreEqual(v1.GetUseCount(), static_cast<uint64_t>(1));
		}

		TEST_METHOD(WeakSharedRefFromSharedRef)
		{
			constexpr int value = 10;

			WeakSharedRef<int> w;

			{
				SharedRef<int> v = CreateSharedRef<int>(value);
				w = v;
				Assert::AreEqual(v.GetUseCount(), static_cast<uint64_t>(1));
				Assert::IsTrue(w.IsValid());

				SharedRef<int> v2 = w.Lock();
				Assert::AreEqual(v.GetUseCount(), static_cast<uint64_t>(2));
				Assert::AreEqual(v.Get(), v2.Get());
			}

			Assert::IsFalse(w.IsValid());
		}

		TEST_METHOD(Inheritance)
		{
			constexpr int a = 12;
			constexpr int b = 213;

			SharedRef<A> sa;
			Assert::IsFalse(sa.IsValid());

			{
				SharedRef<B> sb = CreateSharedRef<B>(a, b);
				Assert::AreEqual(sb->a, a);
				Assert::AreEqual(sb->b, b);

				sa = sb;
				Assert::AreEqual(sa->a, a);
			}

			Assert::IsTrue(sa.IsValid());
		}
	};
}