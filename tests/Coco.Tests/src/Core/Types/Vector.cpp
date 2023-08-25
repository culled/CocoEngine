#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Vector.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(VectorTests)
	{
		TEST_METHOD(Vector2IntCreate)
		{
			constexpr int x = 123;
			constexpr int y = -8932;

			Vector2Int v(x, y);

			Assert::AreEqual(v.X, x);
			Assert::AreEqual(v.Y, y);
		}

		TEST_METHOD(Vector2IntParse)
		{
			Vector2Int v(-123, 9800);
			string t = v.ToString();
			Vector2Int v2 = Vector2Int::Parse(t);

			Assert::IsTrue(v == v2);
		}

		TEST_METHOD(Vector2Create)
		{
			constexpr double x = 0.12;
			constexpr double y = 123.123;

			Vector2 v(x, y);

			Assert::IsTrue(Math::Approximately(v.X, x));
			Assert::IsTrue(Math::Approximately(v.Y, y));
		}

		TEST_METHOD(Vector2Length)
		{
			Vector2 v(2.0, 0.0);

			Assert::IsTrue(Math::Approximately(v.GetLength(), 2.0));
		}

		TEST_METHOD(Vector2Normalize)
		{
			Vector2 v(232.23, 675.2);

			v.Normalize();

			Assert::IsTrue(Math::Approximately(v.GetLength(), 1.0));
		}

		TEST_METHOD(Vector2Dot)
		{
			Vector2 up = Vector2::Up;
			Vector2 right = Vector2::Right;

			Assert::IsTrue(Math::Approximately(up.Dot(up), 1.0));
			Assert::IsTrue(Math::Approximately(up.Dot(-up), -1.0));
			Assert::IsTrue(Math::Approximately(up.Dot(right), 0.0));
			Assert::IsTrue(Math::Approximately(up.Dot(-right), 0.0));
		}

		TEST_METHOD(Vector2Distance)
		{
			Vector2 l = Vector2::Left;
			Vector2 r = Vector2::Right;

			Assert::IsTrue(Math::Approximately(Vector2::DistanceBetween(l, r), 2.0));
		}

		TEST_METHOD(Vector2Project)
		{
			Vector2 v(2.0, 2.0);
			Vector2 n = Vector2::Up;

			Vector2 p = v.Project(n);

			Assert::IsTrue(Math::Approximately(n.Dot(p.Normalized()), 1.0));
			Assert::IsTrue(Math::Approximately(p.GetLength(), 2.0));
		}

		TEST_METHOD(Vector2Reflect)
		{
			Vector2 i(1.0, -1.0);
			Vector2 n = Vector2::Up;

			Vector2 r = i.Reflect(n);

			Assert::IsTrue(Math::Approximately(i.GetLength(), r.GetLength()));
			Assert::IsTrue(Math::Approximately(r.X, 1.0));
			Assert::IsTrue(Math::Approximately(r.Y, 1.0));
		}

		TEST_METHOD(Vector2Parse)
		{
			Vector2 v(12.3, -123.223);
			string t = v.ToString();
			Vector2 v2 = Vector2::Parse(t);

			Assert::IsTrue(v.Equals(v2));
		}

		TEST_METHOD(Vector3Create)
		{
			constexpr double x = 0.12;
			constexpr double y = 123.123;
			constexpr double z = -1231.2;

			Vector3 v(x, y, z);

			Assert::IsTrue(Math::Approximately(v.X, x));
			Assert::IsTrue(Math::Approximately(v.Y, y));
			Assert::IsTrue(Math::Approximately(v.Z, z));
		}

		TEST_METHOD(Vector3Length)
		{
			Vector3 v(0.0, 0.0, 3.0);

			Assert::IsTrue(Math::Approximately(v.GetLength(), 3.0));
		}

		TEST_METHOD(Vector3Normalize)
		{
			Vector3 v(234.0, 21.0, 213.0);

			v.Normalize();

			Assert::IsTrue(Math::Approximately(v.GetLength(), 1.0));
		}

		TEST_METHOD(Vector3Dot)
		{
			Vector3 up = Vector3::Up;
			Vector3 right = Vector3::Right;
			Vector3 forward = Vector3::Forwards;

			Assert::IsTrue(Math::Approximately(up.Dot(up), 1.0));
			Assert::IsTrue(Math::Approximately(up.Dot(-up), -1.0));
			Assert::IsTrue(Math::Approximately(up.Dot(right), 0.0));
			Assert::IsTrue(Math::Approximately(up.Dot(-right), 0.0));
			Assert::IsTrue(Math::Approximately(up.Dot(forward), 0.0));
			Assert::IsTrue(Math::Approximately(up.Dot(-forward), 0.0));
		}

		TEST_METHOD(Vector3Distance)
		{
			Vector3 l = Vector3::Left;
			Vector3 r = Vector3::Right;

			Assert::IsTrue(Math::Approximately(Vector3::DistanceBetween(l, r), 2.0));
		}

		TEST_METHOD(Vector3Project)
		{
			Vector3 v(2.0, 2.0, 2.0);
			Vector3 n = Vector3::Up;

			Vector3 p = v.Project(n);

			Assert::IsTrue(Math::Approximately(n.Dot(p.Normalized()), 1.0));
			Assert::IsTrue(Math::Approximately(p.GetLength(), 2.0));
		}

		TEST_METHOD(Vector3Reflect)
		{
			Vector3 i(1.0, 1.0, -1.0);
			Vector3 n = Vector3::Up;

			Vector3 r = i.Reflect(n);

			Assert::IsTrue(Math::Approximately(i.GetLength(), r.GetLength()));
			Assert::IsTrue(Math::Approximately(r.X, 1.0));
			Assert::IsTrue(Math::Approximately(r.Y, 1.0));
			Assert::IsTrue(Math::Approximately(r.Z, 1.0));
		}

		TEST_METHOD(Vector3Cross)
		{
			Vector3 up = Vector3::Up;
			Vector3 forward = Vector3::Forwards;
			Vector3 right = Vector3::Cross(forward, up);

			Assert::IsTrue(right.Equals(Vector3::Right));
		}

		TEST_METHOD(Vector3Parse)
		{
			Vector3 v(12.3, -123.223, 12312.3);
			string t = v.ToString();
			Vector3 v2 = Vector3::Parse(t);

			Assert::IsTrue(v.Equals(v2));
		}

		TEST_METHOD(Vector4Create)
		{
			constexpr double x = 0.12;
			constexpr double y = 123.123;
			constexpr double z = -1231.2;
			constexpr double w = -653.2;

			Vector4 v(x, y, z, w);

			Assert::IsTrue(Math::Approximately(v.X, x));
			Assert::IsTrue(Math::Approximately(v.Y, y));
			Assert::IsTrue(Math::Approximately(v.Z, z));
			Assert::IsTrue(Math::Approximately(v.W, w));
		}

		TEST_METHOD(Vector4Length)
		{
			Vector4 v(0.0, 0.0, 0.0, 2.0);

			Assert::IsTrue(Math::Approximately(v.GetLength(), 2.0));
		}

		TEST_METHOD(Vector4Normalize)
		{
			Vector4 v(234.0, 21.123, 213.0, -12312.23);

			v.Normalize();

			Assert::IsTrue(Math::Approximately(v.GetLength(), 1.0));
		}

		TEST_METHOD(Vector4Dot)
		{
			Vector4 x(1.0, 0.0, 0.0, 0.0);
			Vector4 y(0.0, 1.0, 0.0, 0.0);
			Vector4 z(0.0, 0.0, 1.0, 0.0);
			Vector4 w(0.0, 0.0, 0.0, 1.0);

			Assert::IsTrue(Math::Approximately(x.Dot(x), 1.0));
			Assert::IsTrue(Math::Approximately(x.Dot(-x), -1.0));
			Assert::IsTrue(Math::Approximately(x.Dot(y), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(-y), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(z), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(-z), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(w), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(-w), 0.0));
		}

		TEST_METHOD(Vector4Parse)
		{
			Vector4 v(12.3, -123.223, 12312.3, -1023.23);
			string t = v.ToString();
			Vector4 v2 = Vector4::Parse(t);

			Assert::IsTrue(v.Equals(v2));
		}
	};
}