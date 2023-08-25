#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Rect.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(RectTests)
	{
		TEST_METHOD(RectIntCreate)
		{
			Vector2Int offset(-1, 1);
			SizeInt size(2, 2);
			RectInt r(offset, size);

			Assert::IsTrue(r.Offset == offset);
			Assert::IsTrue(r.Size == size);

			Assert::IsTrue(r.GetCenter().Equals(Vector2(0.0, 2.0)));

			Assert::AreEqual(r.GetBottom(), 1);
			Assert::AreEqual(r.GetTop(), 3);
			Assert::AreEqual(r.GetLeft(), -1);
			Assert::AreEqual(r.GetRight(), 1);
		}

		TEST_METHOD(RectIntExpand)
		{
			RectInt r(Vector2Int(0, 0), Vector2Int(1, 1));

			r.Expand(Vector2Int(-1, -1));

			Assert::IsTrue(r.Offset == Vector2Int(-1, -1));
			Assert::IsTrue(r.Size == SizeInt(2, 2));

			r.Expand(Vector2Int(2, 2));

			Assert::IsTrue(r.Offset == Vector2Int(-1, -1));
			Assert::IsTrue(r.Size == SizeInt(3, 3));
		}

		TEST_METHOD(RectIntIntersectWithPoint)
		{
			RectInt r(Vector2Int(-1, -1), Vector2Int(1, 1));

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

		TEST_METHOD(RectIntIntersectWithRectInt)
		{
			RectInt r(Vector2Int(0, 0), Vector2Int(1, 1));

			for (int x = -2; x <= 2; x++)
			{
				for (int y = -2; y <= 2; y++)
				{
					RectInt t(Vector2Int(x, y), SizeInt(1, 1));

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

		TEST_METHOD(RectCreate)
		{
			Vector2 offset(-1.0, 1.0);
			Size size(2.0, 2.0);
			Rect r(offset, size);

			Assert::IsTrue(r.Offset.Equals(offset));
			Assert::IsTrue(r.Size.Equals(size));

			Assert::IsTrue(r.GetCenter().Equals(Vector2(0.0, 2.0)));

			Assert::IsTrue(Math::Approximately(r.GetBottom(), 1.0));
			Assert::IsTrue(Math::Approximately(r.GetTop(), 3.0));
			Assert::IsTrue(Math::Approximately(r.GetLeft(), -1.0));
			Assert::IsTrue(Math::Approximately(r.GetRight(), 1.0));
		}

		TEST_METHOD(RectCreateWithCenter)
		{
			Vector2 center(2.2, -0.12);
			Size size(2.0, 2.0);
			Rect r = Rect::CreateWithCenterAndSize(center, size);

			Assert::IsTrue(r.GetCenter().Equals(center));
			Assert::IsTrue(r.Size.Equals(size));
		}

		TEST_METHOD(RectExpand)
		{
			Rect r(Vector2(0.0, 0.0), Vector2(1.0, 1.0));

			r.Expand(Vector2(-1.0, -1.0));

			Assert::IsTrue(r.Offset.Equals(Vector2(-1.0, -1.0)));
			Assert::IsTrue(r.Size.Equals(Size(2.0, 2.0)));

			r.Expand(Vector2(2.0, 2.0));

			Assert::IsTrue(r.Offset.Equals(Vector2(-1.0, -1.0)));
			Assert::IsTrue(r.Size.Equals(Size(3.0, 3.0)));
		}

		TEST_METHOD(RectIntersectWithPoint)
		{
			Rect r(Vector2(-0.5, -0.5), Vector2(0.5, 0.5));

			for (int x = -2; x <= 2; x++)
			{
				for (int y = -2; y <= 2; y++)
				{
					Vector2 v(x * 0.5, y * 0.5);

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
			Rect r(Vector2(-0.5, -0.5), Vector2(0.5, 0.5));

			for (int x = -2; x <= 2; x++)
			{
				for (int y = -2; y <= 2; y++)
				{
					Rect t = Rect::CreateWithCenterAndSize(Vector2(x, y), Size(1.0, 1.0));

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
			Rect r(Vector2(-0.5, -0.5), Vector2(0.5, 0.5));

			Rect r2(Vector2(0.0, 0.0), Vector2(1.0, 1.0));
			Rect i = r.GetIntersection(r2);

			Assert::IsTrue(Math::Approximately(i.GetLeft(), Math::Max(r.GetLeft(), r2.GetLeft())));
			Assert::IsTrue(Math::Approximately(i.GetRight(), Math::Min(r.GetRight(), r2.GetRight())));
			Assert::IsTrue(Math::Approximately(i.GetBottom(), Math::Max(r.GetBottom(), r2.GetBottom())));
			Assert::IsTrue(Math::Approximately(i.GetTop(), Math::Min(r.GetTop(), r2.GetTop())));
		}

		TEST_METHOD(RectClosestPointToPoint)
		{
			Rect r(Vector2(0.0, 0.0), Vector2(2.0, 2.0));
			Rect::RectangleSide side;

			Vector2 right = r.GetClosestPoint(Vector2(3.0, 1.0), &side);
			Assert::IsTrue(Math::Approximately(right.X, r.GetRight()));
			Assert::IsTrue(Math::Approximately(right.Y, 1.0));
			Assert::IsTrue(side == Rect::RectangleSide::Right);

			Vector2 left = r.GetClosestPoint(Vector2(-1.0, 1.0), &side);
			Assert::IsTrue(Math::Approximately(left.X, r.GetLeft()));
			Assert::IsTrue(Math::Approximately(left.Y, 1.0));
			Assert::IsTrue(side == Rect::RectangleSide::Left);

			Vector2 top = r.GetClosestPoint(Vector2(1.0, 3.0), &side);
			Assert::IsTrue(Math::Approximately(top.X, 1.0));
			Assert::IsTrue(Math::Approximately(top.Y, r.GetTop()));
			Assert::IsTrue(side == Rect::RectangleSide::Top);

			Vector2 bottom = r.GetClosestPoint(Vector2(1.0, -1.0), &side);
			Assert::IsTrue(Math::Approximately(bottom.X, 1.0));
			Assert::IsTrue(Math::Approximately(bottom.Y, r.GetBottom()));
			Assert::IsTrue(side == Rect::RectangleSide::Bottom);
		}

		TEST_METHOD(RectClosestPointToRect)
		{
			Rect r(Vector2(0.0, 0.0), Vector2(2.0, 2.0));
			Rect::RectangleSide side;

			Vector2 right = r.GetClosestPoint(Rect(Vector2(3.0, 0.0), Size(2.0, 2.0)), &side);
			Assert::IsTrue(Math::Approximately(right.X, r.GetRight()));
			Assert::IsTrue(Math::Approximately(right.Y, 1.0));
			Assert::IsTrue(side == Rect::RectangleSide::Right);

			Vector2 left = r.GetClosestPoint(Rect(Vector2(-3.0, 0.0), Size(2.0, 2.0)), &side);
			Assert::IsTrue(Math::Approximately(left.X, r.GetLeft()));
			Assert::IsTrue(Math::Approximately(left.Y, 1.0));
			Assert::IsTrue(side == Rect::RectangleSide::Left);

			Vector2 top = r.GetClosestPoint(Rect(Vector2(0.0, 3.0), Size(2.0, 2.0)), &side);
			Assert::IsTrue(Math::Approximately(top.X, 1.0));
			Assert::IsTrue(Math::Approximately(top.Y, r.GetTop()));
			Assert::IsTrue(side == Rect::RectangleSide::Top);

			Vector2 bottom = r.GetClosestPoint(Rect(Vector2(0.0, -3.0), Size(2.0, 2.0)), &side);
			Assert::IsTrue(Math::Approximately(bottom.X, 1.0));
			Assert::IsTrue(Math::Approximately(bottom.Y, r.GetBottom()));
			Assert::IsTrue(side == Rect::RectangleSide::Bottom);
		}
	};
}