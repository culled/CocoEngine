#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Color.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(ColorTests)
	{
		TEST_METHOD(LinearGammaConversions)
		{
			Color g(0.5, 0.5, 0.5, 0.5, false);

			Color l = g.AsLinear();
			Assert::IsTrue(l.IsLinear);

			Color g2 = l.AsGamma();
			Assert::IsFalse(g2.IsLinear);

			Assert::IsTrue(Math::Approximately(g.R, g2.R));
			Assert::IsTrue(Math::Approximately(g.G, g2.G));
			Assert::IsTrue(Math::Approximately(g.B, g2.B));
			Assert::IsTrue(Math::Approximately(g.A, g2.A));
		}

		TEST_METHOD(ColorParsing)
		{
			Color c(0.2, 0.3, 0.4, 0.7, true);
			string cText = c.ToString();
			Color c2 = Color::Parse(cText);

			Assert::IsTrue(Math::Approximately(c.R, c2.R));
			Assert::IsTrue(Math::Approximately(c.G, c2.G));
			Assert::IsTrue(Math::Approximately(c.B, c2.B));
			Assert::IsTrue(Math::Approximately(c.A, c2.A));
			Assert::AreEqual(c.IsLinear, c2.IsLinear);
		}
	};
}