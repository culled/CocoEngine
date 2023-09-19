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

			Assert::IsTrue(Math::Equal(q.X, x));
			Assert::IsTrue(Math::Equal(q.Y, y));
			Assert::IsTrue(Math::Equal(q.Z, z));
			Assert::IsTrue(Math::Equal(q.W, w));
		}

		TEST_METHOD(GetNormal)
		{
			Quaternion q(1.0, 0.0, 0.0, 0.0);

			Assert::IsTrue(Math::Equal(q.GetNormal(), 1.0));
		}

		TEST_METHOD(Normalize)
		{
			Quaternion q(2.0, -0.5, 0.223, 0.8);

			q.Normalize();

			Assert::IsTrue(Math::Equal(q.GetNormal(), 1.0));
		}

		TEST_METHOD(Conjugate)
		{
			Quaternion q(0.2, 0.3, 0.4, 0.5);
			Quaternion q2 = q.Conjugate();

			Assert::IsTrue(Math::Equal(q.X, -q2.X));
			Assert::IsTrue(Math::Equal(q.Y, -q2.Y));
			Assert::IsTrue(Math::Equal(q.Z, -q2.Z));
			Assert::IsTrue(Math::Equal(q.W, q2.W));
		}

		TEST_METHOD(Dot)
		{
			Quaternion x(1.0, 0.0, 0.0, 0.0);
			Quaternion y(0.0, 1.0, 0.0, 0.0);
			Quaternion z(0.0, 0.0, 1.0, 0.0);
			Quaternion w(0.0, 0.0, 0.0, 1.0);

			Assert::IsTrue(Math::Equal(x.Dot(x), 1.0));
			Assert::IsTrue(Math::Equal(x.Dot(x.Conjugate()), -1.0));
			Assert::IsTrue(Math::Equal(x.Dot(y), 0.0));
			Assert::IsTrue(Math::Equal(x.Dot(y.Conjugate()), 0.0));
			Assert::IsTrue(Math::Equal(x.Dot(z), 0.0));
			Assert::IsTrue(Math::Equal(x.Dot(z.Conjugate()), 0.0));
			Assert::IsTrue(Math::Equal(x.Dot(w), 0.0));
			Assert::IsTrue(Math::Equal(x.Dot(w.Conjugate()), 0.0));
		}

		TEST_METHOD(EulerAngles)
		{
			Vector3 e(Math::DegToRad(45.0), Math::DegToRad(65.0), Math::DegToRad(-24.0));

			Quaternion q(e);

			Vector3 e2 = q.ToEulerAngles();

			Assert::IsTrue(e.Equals(e2, 0.00001));
		}

		TEST_METHOD(RotatingQuaternions)
		{
			Quaternion q;
			q *= Quaternion(Vector3::Up, Math::DegToRad(90.0));

			Vector3 e = q.ToEulerAngles();

			Assert::IsTrue(e.Equals(Vector3(0.0, Math::DegToRad(90.0), 0.0)));
		}

		TEST_METHOD(RotatingVectors)
		{
			Vector3 v = Vector3::Forward;

			v = Quaternion(Vector3::Up, Math::DegToRad(90.0)) * v;

			Assert::IsTrue(v.Equals(Vector3::Left));
		}
	};
}