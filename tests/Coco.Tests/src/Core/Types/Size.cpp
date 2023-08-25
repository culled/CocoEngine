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

		TEST_METHOD(SizeIntParse)
		{
			SizeInt s(90812, 1234);
			string t = s.ToString();
			SizeInt s2 = SizeInt::Parse(t);

			Assert::IsTrue(s == s2);
		}

		TEST_METHOD(SizeCreate)
		{
			constexpr double w = 1231.12;
			constexpr double h = 90.32;

			Size s(w, h);

			Assert::IsTrue(Math::Approximately(s.Width, w));
			Assert::IsTrue(Math::Approximately(s.Height, h));
		}

		TEST_METHOD(SizeParse)
		{
			Size s(8912.23, 23.233);
			string t = s.ToString();
			Size s2 = Size::Parse(t);

			Assert::IsTrue(s.Equals(s2));
		}
	};
}