#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Plane.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(PlaneTests)
	{
		TEST_METHOD(CreateWithOriginAndNormal)
		{
			Vector3 o(1.0, 0.0, 0.0);
			Vector3 n = Vector3::Right;

			Plane p(o, n);

			Assert::IsTrue(Math::Equal(p.Distance, o.X));
			Assert::IsTrue(p.Normal.Equals(n));
		}

		TEST_METHOD(CreateFromPoints)
		{
			Vector3 p0(-1.0, 0.0, 0.0);
			Vector3 p1(0.0, 0.0, -1.0);
			Vector3 p2(1.0, 0.0, 0.0);

			Plane p(p0, p1, p2);

			Vector3 a = (p0 + p1 + p2) / 3.0;
			Assert::IsTrue(Math::Equal(p.Distance, 0.0));
			Assert::IsTrue(p.Normal.Equals(Vector3::Up));
		}

		TEST_METHOD(GetClosestPoint)
		{
			Plane plane(Vector3::Zero, Vector3::Up);
			Vector3 point(1.0, 1.0, 1.0);

			Vector3 c = plane.GetClosestPoint(point);

			Assert::IsTrue(Math::Equal(c.X, point.X));
			Assert::IsTrue(Math::Equal(c.Y, 0.0));
			Assert::IsTrue(Math::Equal(c.Z, point.Z));
		}

		TEST_METHOD(GetDistance)
		{
			Plane plane(Vector3::Zero, Vector3::Right);
			Vector3 point(3.0, 1.0, 1.0);

			double d = plane.GetDistance(point);

			Assert::IsTrue(Math::Equal(d, point.X));

			point = Vector3(-3.0, 1.0, 1.0);

			d = plane.GetDistance(point);

			Assert::IsTrue(Math::Equal(d, -point.X));
		}

		TEST_METHOD(IsOnNormalSide)
		{
			Plane plane(Vector3::Zero, Vector3::Forward);
			Vector3 point(3.0, 1.0, -1.0);

			Assert::IsTrue(plane.IsOnNormalSide(point));

			point *= -1.0;

			Assert::IsFalse(plane.IsOnNormalSide(point));
		}
	};
}