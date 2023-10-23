#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Vector.h>
#include <Coco/Core/Math/Math.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(VectorTests)
	{
		TEST_METHOD(Vector2Create)
		{
			constexpr double x = 0.12;
			constexpr double y = 123.123;

			Vector2 v(x, y);

			Assert::IsTrue(Math::Equal(v.X, x));
			Assert::IsTrue(Math::Equal(v.Y, y));
		}

		TEST_METHOD(Vector2Equals)
		{
			constexpr double x = -0.23;
			constexpr double y = 2.1;

			Vector2 v(x, y);
			Vector2 v2(x, y);
			Vector2 v3(x * 2, y * 2);

			Assert::IsTrue(v.Equals(v2));
			Assert::IsFalse(v.Equals(v3));
		}

		TEST_METHOD(Vector2GetLength)
		{
			constexpr double x = 2.2;
			constexpr double y = -12.2;
			constexpr double l2 = x * x + y * y;
			double l = Math::Sqrt(l2);

			Vector2 v(x, y);

			Assert::IsTrue(Math::Equal(v.GetLengthSquared(), l2));
			Assert::IsTrue(Math::Equal(v.GetLength(), l));
		}

		TEST_METHOD(Vector2Normalize)
		{
			constexpr double x = 3;
			constexpr double y = -3;

			Vector2 v(x, y);
			Assert::IsFalse(Math::Equal(v.GetLength(), 1.0));

			v.Normalize();
			Assert::IsTrue(Math::Equal(v.GetLength(), 1.0));
		}

		TEST_METHOD(Vector2Dot)
		{
			Assert::IsTrue(Math::Equal(Vector2::Up.Dot(Vector2::Up), 1.0));
			Assert::IsTrue(Math::Equal(Vector2::Up.Dot(Vector2::Right), 0.0));
			Assert::IsTrue(Math::Equal(Vector2::Up.Dot(Vector2::Left), 0.0));
			Assert::IsTrue(Math::Equal(Vector2::Up.Dot(Vector2::Down), -1.0));
		}

		TEST_METHOD(Vector2Project)
		{
			Vector2 v(2, 2);
			Vector2 n(0, 1);

			Vector2 p = v.Project(n);

			Assert::IsTrue(Math::Equal(n.Dot(p), 2.0));
		}

		TEST_METHOD(Vector2Reflect)
		{
			Vector2 v(2, -2);

			Vector2 r = v.Reflect(Vector2::Up);

			Assert::IsTrue(Math::Equal(v.GetLength(), r.GetLength()));
			Assert::IsTrue(Math::Equal(v.X, r.X));
			Assert::IsTrue(Math::Equal(v.Y, -r.Y));
		}

		TEST_METHOD(Vector2IntCreate)
		{
			constexpr int x = 123;
			constexpr int y = -8932;

			Vector2Int v(x, y);

			Assert::AreEqual(v.X, x);
			Assert::AreEqual(v.Y, y);
		}
	};
}