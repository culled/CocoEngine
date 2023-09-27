#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Rect.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(RectTests)
	{
		TEST_METHOD(RectCreate)
		{
			Vector2 min(-1.0, -1.0);
			Vector2 max(1.0, 1.0);
			Rect r(min, max);

			Assert::IsTrue(r.Minimum.Equals(min));
			Assert::IsTrue(r.Maximum.Equals(max));
		}

		TEST_METHOD(RectCreateWithCenter)
		{
			Vector2 center(2.2, -0.12);
			Size size(2.0, 2.0);
			Rect r = Rect::FromCenterAndSize(center, size);

			Vector2 c = r.GetCenter();
			Size s = r.GetSize();

			Assert::IsTrue(center.Equals(c));
			Assert::IsTrue(size.Equals(s));
		}

		TEST_METHOD(RectExpand)
		{
			Rect r(Vector2(0.0, 0.0), Vector2(1.0, 1.0));

			r.Expand(Vector2(-1.0, -1.0));

			Assert::IsTrue(r.Minimum.Equals(Vector2(-1.0, -1.0)));
			Assert::IsTrue(r.Maximum.Equals(Vector2(1.0, 1.0)));

			r.Expand(Vector2(2.0, 2.0));

			Assert::IsTrue(r.Minimum.Equals(Vector2(-1.0, -1.0)));
			Assert::IsTrue(r.Maximum.Equals(Vector2(2.0, 2.0)));
		}


		TEST_METHOD(RectIntersectWithPoint)
		{
			Rect r(Vector2(-1.0, -1.0), Vector2(1.0, 1.0));

			for (int x = -2; x <= 2; x++)
			{
				for (int y = -2; y <= 2; y++)
				{
					Vector2 v(x, y);

					if (x > -2 && x < 2 && y > -2 && y < 2)
					{
						Assert::IsTrue(r.Intersects(v));
					}
					else
					{
						Assert::IsFalse(r.Intersects(v));
					}
				}
			}
		}

		TEST_METHOD(RectIntersectWithRect)
		{
			Rect r(Vector2(-1.0, -1.0), Vector2(1.0, 1.0));

			for (int x = -2; x <= 2; x++)
			{
				for (int y = -2; y <= 2; y++)
				{
					Rect t = Rect::FromCenterAndSize(Vector2(x, y), Size(0.5, 0.5));

					if (x > -2 && x < 2 && y > -2 && y < 2)
					{
						Assert::IsTrue(r.Intersects(t));
						Assert::IsTrue(t.Intersects(r));
					}
					else
					{
						Assert::IsFalse(r.Intersects(t));
						Assert::IsFalse(t.Intersects(r));
					}
				}
			}
		}

		TEST_METHOD(RectIntersection)
		{
			Rect r(Vector2(-2.0, -2.0), Vector2(1.0, 1.0));

			Rect r2(Vector2(-1.0, -1.0), Vector2(2.0, 2.0));
			Rect i = r.GetIntersection(r2);

			Assert::IsTrue(i.Minimum.Equals(Vector2(-1.0, -1.0)));
			Assert::IsTrue(i.Maximum.Equals(Vector2(1.0, 1.0)));
		}

		TEST_METHOD(RectClosestPointToPoint)
		{
			Rect r(Vector2(-1.0, -1.0), Vector2(1.0, 1.0));
			RectangleSide side;

			Vector2 right = r.GetClosestPoint(Vector2(3.0, 0.0), &side);
			Assert::IsTrue(right.Equals(Vector2(r.GetRight(), 0.0)));
			Assert::IsTrue(side == RectangleSide::Right);

			Vector2 left = r.GetClosestPoint(Vector2(-3.0, 0.0), &side);
			Assert::IsTrue(left.Equals(Vector2(r.GetLeft(), 0.0)));
			Assert::IsTrue(side == RectangleSide::Left);

			Vector2 bottom = r.GetClosestPoint(Vector2(0.0, -3.0), &side);
			Assert::IsTrue(bottom.Equals(Vector2(0.0, r.GetBottom())));
			Assert::IsTrue(side == RectangleSide::Bottom);

			Vector2 top = r.GetClosestPoint(Vector2(0.0, 3.0), &side);
			Assert::IsTrue(top.Equals(Vector2(0.0, r.GetTop())));
			Assert::IsTrue(side == RectangleSide::Top);
		}
	};
}