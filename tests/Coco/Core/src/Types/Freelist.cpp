#include "pch.h"
#include "CppUnitTest.h"
#include <Coco/Core/Types/Freelist.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Coco;

namespace Coco::Core::Types
{
	TEST_CLASS(FreelistTests)
	{
	public:
		TEST_METHOD(AllocateList)
		{
			constexpr uint64 size = 512;
			Freelist list(size);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(AllocateAndFreeOneEntry)
		{
			constexpr uint64 size = 512;
			constexpr uint64 allocateSize = 64;

			Freelist list(size);
			FreelistNode block;
			Assert::IsTrue(list.Allocate(allocateSize, block));

			Assert::IsTrue(block.Offset == 0);
			Assert::AreEqual(block.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize);

			list.Free(block);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(AllocateAndFreeMultipleEntries)
		{
			constexpr uint64 size = 512;
			constexpr uint64 allocateSize = 64;

			Freelist list(size);

			FreelistNode block1;
			Assert::IsTrue(list.Allocate(allocateSize, block1));
			Assert::IsTrue(block1.Offset == 0);
			Assert::AreEqual(block1.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize);

			FreelistNode block2;
			Assert::IsTrue(list.Allocate(allocateSize, block2));
			Assert::AreEqual(block2.Offset, allocateSize);
			Assert::AreEqual(block2.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize * 2);

			FreelistNode block3;
			Assert::IsTrue(list.Allocate(allocateSize, block3));
			Assert::AreEqual(block3.Offset, allocateSize * 2);
			Assert::AreEqual(block3.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize * 3);

			list.Free(block2);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize * 2);

			list.Free(block1);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize);

			list.Free(block3);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(AllocateAndFreeVaryingSizeEntries)
		{
			constexpr uint64 size = 512;
			Freelist list(size);

			constexpr uint64 block1Size = 64;
			FreelistNode block1;
			Assert::IsTrue(list.Allocate(block1Size, block1));
			Assert::IsTrue(block1.Offset == 0);
			Assert::AreEqual(block1.Size, block1Size);

			constexpr uint64 block2Size = 128;
			FreelistNode block2;
			Assert::IsTrue(list.Allocate(block2Size, block2));
			Assert::AreEqual(block2.Offset, block1Size);
			Assert::AreEqual(block2.Size, block2Size);

			constexpr uint64 block3Size = 256;
			FreelistNode block3;
			Assert::IsTrue(list.Allocate(block3Size, block3));
			Assert::AreEqual(block3.Offset, block1Size + block2Size);
			Assert::AreEqual(block3.Size, block3Size);

			list.Free(block1);
			list.Free(block2);
			Assert::AreEqual(list.GetFreeSpace(), size - block3Size);

			constexpr uint64_t block4Size = block1Size + block2Size;
			FreelistNode block4;
			Assert::IsTrue(list.Allocate(block4Size, block4));
			Assert::IsTrue(block4.Offset == 0);
			Assert::AreEqual(block4.Size, block4Size);

			constexpr uint64_t block5Size = size;
			FreelistNode block5;
			Assert::IsFalse(list.Allocate(block5Size, block5));

			list.Free(block3);
			list.Free(block4);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(Resize)
		{
			Freelist list(64);

			FreelistNode block1;
			Assert::IsTrue(list.Allocate(32, block1));

			list.Resize(128);
			Assert::IsTrue(list.GetFreeSpace() == 96);

			FreelistNode block2;
			Assert::IsTrue(list.Allocate(64, block2));
			Assert::IsTrue(list.GetFreeSpace() == 32);

			list.Resize(96);
			Assert::IsTrue(list.GetFreeSpace() == 0);
		}
	};
}