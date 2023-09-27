#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/BoundingBox.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(BoundingBoxTests)
	{
		TEST_METHOD(BoundingBoxCreate)
		{
			Vector3 min(-1.0, -1.0, -1.0);
			Vector3 max(1.0, 1.0, 1.0);
			BoundingBox b(min, max);

			Assert::IsTrue(b.Minimum.Equals(min));
			Assert::IsTrue(b.Maximum.Equals(max));
		}

		TEST_METHOD(BoundingBoxCreateWithCenter)
		{
			Vector3 center(2.2, -0.12, 1.1);
			Vector3 size(2.0, 2.0, 2.0);
			BoundingBox b = BoundingBox::FromCenterAndSize(center, size);

			Vector3 c = b.GetCenter();
			Vector3 s = b.GetSize();

			Assert::IsTrue(center.Equals(c));
			Assert::IsTrue(size.Equals(s));
		}

		TEST_METHOD(BoundingBoxExpand)
		{
			BoundingBox b(Vector3(0.0, 0.0, 0.0), Vector3(1.0, 1.0, 1.0));

			b.Expand(Vector3(-1.0, -1.0, -1.0));

			Assert::IsTrue(b.Minimum.Equals(Vector3(-1.0, -1.0, -1.0)));
			Assert::IsTrue(b.Maximum.Equals(Vector3(1.0, 1.0, 1.0)));

			b.Expand(Vector3(2.0, 2.0, 2.0));

			Assert::IsTrue(b.Minimum.Equals(Vector3(-1.0, -1.0, -1.0)));
			Assert::IsTrue(b.Maximum.Equals(Vector3(2.0, 2.0, 2.0)));
		}

		TEST_METHOD(BoundingBoxIntersectWithPoint)
		{
			BoundingBox b(Vector3(-1.0, -1.0, -1.0), Vector3(1.0, 1.0, 1.0));

			for (int x = -2; x <= 2; x++)
			{
				for (int y = -2; y <= 2; y++)
				{
					for (int z = -2; z <= 2; z++)
					{
						Vector3 v(x, y, z);

						if (x > -2 && x < 2 && y > -2 && y < 2 && z > -2 && z < 2)
						{
							Assert::IsTrue(b.Intersects(v));
						}
						else
						{
							Assert::IsFalse(b.Intersects(v));
						}
					}
				}
			}
		}

		TEST_METHOD(BoundingBoxIntersectWithBox)
		{
			BoundingBox b(Vector3(-1.0, -1.0, -1.0), Vector3(1.0, 1.0, 1.0));

			for (int x = -2; x <= 2; x++)
			{
				for (int y = -2; y <= 2; y++)
				{
					for (int z = -2; z <= 2; z++)
					{
						BoundingBox v = BoundingBox::FromCenterAndSize(Vector3(x, y, z), Vector3(0.5, 0.5, 0.5));

						if (x > -2 && x < 2 && y > -2 && y < 2 && z > -2 && z < 2)
						{
							Assert::IsTrue(b.Intersects(v));
						}
						else
						{
							Assert::IsFalse(b.Intersects(v));
						}
					}
				}
			}
		}
	};
}