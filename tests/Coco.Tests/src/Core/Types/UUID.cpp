#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/UUID.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(UUIDTests)
	{
		TEST_METHOD(ParseUUID)
		{
			UUID u = UUID::CreateV4();
			string t = u.ToString();
			UUID u2(t);

			Assert::IsTrue(u == u2);
		}
	};
}