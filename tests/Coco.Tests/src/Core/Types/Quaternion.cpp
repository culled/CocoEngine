#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Quaternion.h>
#include <Coco/Core/Types/Vector.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(QuaternionTests)
	{
		TEST_METHOD(CreateQuaternion)
		{
			constexpr double x = 0.1123;
			constexpr double y = 0.5123;
			constexpr double z = -0.231;
			constexpr double w = -0.87;

			Quaternion q(x, y, z, w);

			Assert::IsTrue(Math::Approximately(q.X, x));
			Assert::IsTrue(Math::Approximately(q.Y, y));
			Assert::IsTrue(Math::Approximately(q.Z, z));
			Assert::IsTrue(Math::Approximately(q.W, w));
		}

		TEST_METHOD(GetNormal)
		{
			Quaternion q(1.0, 0.0, 0.0, 0.0);

			Assert::IsTrue(Math::Approximately(q.GetNormal(), 1.0));
		}

		TEST_METHOD(Normalize)
		{
			Quaternion q(2.0, 0.0, 0.0, 0.0);

			q.Normalize();

			Assert::IsTrue(Math::Approximately(q.GetNormal(), 1.0));
		}

		TEST_METHOD(Conjugate)
		{
			Quaternion q(0.2, 0.3, 0.4, 0.5);
			Quaternion q2 = q.Conjugate();

			Assert::IsTrue(Math::Approximately(q.X, -q2.X));
			Assert::IsTrue(Math::Approximately(q.Y, -q2.Y));
			Assert::IsTrue(Math::Approximately(q.Z, -q2.Z));
			Assert::IsTrue(Math::Approximately(q.W, q2.W));
		}

		TEST_METHOD(Dot)
		{
			Quaternion x(1.0, 0.0, 0.0, 0.0);
			Quaternion y(0.0, 1.0, 0.0, 0.0);
			Quaternion z(0.0, 0.0, 1.0, 0.0);
			Quaternion w(0.0, 0.0, 0.0, 1.0);

			Assert::IsTrue(Math::Approximately(x.Dot(x), 1.0));
			Assert::IsTrue(Math::Approximately(x.Dot(x.Conjugate()), -1.0));
			Assert::IsTrue(Math::Approximately(x.Dot(y), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(y.Conjugate()), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(z), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(z.Conjugate()), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(w), 0.0));
			Assert::IsTrue(Math::Approximately(x.Dot(w.Conjugate()), 0.0));
		}

		TEST_METHOD(EulerAngles)
		{
			Vector3 e(Math::Deg2Rad(45.0), Math::Deg2Rad(65.0), 0.0);

			Quaternion q(e);

			Vector3 e2 = q.ToEulerAngles();

			Assert::IsTrue(e.Equals(e2));
		}

		TEST_METHOD(RotatingQuaternions)
		{
			Quaternion q;
			q *= Quaternion(Vector3::Up, Math::Deg2Rad(90.0));

			Vector3 e = q.ToEulerAngles();

			Assert::IsTrue(e.Equals(Vector3(0.0, 0.0, Math::Deg2Rad(90.0))));
		}

		TEST_METHOD(RotatingVectors)
		{
			Vector3 v = Vector3::Forwards;

			v = Quaternion(Vector3::Up, Math::Deg2Rad(90.0)) * v;

			Assert::IsTrue(v.Equals(-Vector3::Right));
		}
	};
}