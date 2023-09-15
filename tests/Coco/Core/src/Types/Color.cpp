#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Math/Math.h>
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

			Assert::IsTrue(Math::Equal(g.R, g2.R));
			Assert::IsTrue(Math::Equal(g.G, g2.G));
			Assert::IsTrue(Math::Equal(g.B, g2.B));
			Assert::IsTrue(Math::Equal(g.A, g2.A));
		}
	};
}