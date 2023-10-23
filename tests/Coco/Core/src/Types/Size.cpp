#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Size.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(SizeTests)
	{
		TEST_METHOD(SizeIntCreate)
		{
			constexpr int w = 5423;
			constexpr int h = 42;

			SizeInt s(w, h);

			Assert::AreEqual(s.Width, w);
			Assert::AreEqual(s.Height, h);
		}

		TEST_METHOD(SizeCreate)
		{
			constexpr double w = 1231.12;
			constexpr double h = 90.32;

			Size s(w, h);

			Assert::IsTrue(Math::Equal(s.Width, w));
			Assert::IsTrue(Math::Equal(s.Height, h));
		}
	};
}