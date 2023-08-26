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
			constexpr uint64_t size = 512;
			Freelist list(size);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(AllocateAndFreeOneEntry)
		{
			constexpr uint64_t size = 512;
			constexpr uint64_t allocateSize = 64;

			Freelist list(size);
			FreelistAllocatedBlock block;
			Assert::IsTrue(list.Allocate(allocateSize, block));

			Assert::AreEqual(block.Offset, static_cast<uint64_t>(0));
			Assert::AreEqual(block.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize);

			list.Return(block);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(AllocateAndFreeMultipleEntries)
		{
			constexpr uint64_t size = 512;
			constexpr uint64_t allocateSize = 64;

			Freelist list(size);

			FreelistAllocatedBlock block1;
			Assert::IsTrue(list.Allocate(allocateSize, block1));
			Assert::AreEqual(block1.Offset, static_cast<uint64_t>(0));
			Assert::AreEqual(block1.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize);

			FreelistAllocatedBlock block2;
			Assert::IsTrue(list.Allocate(allocateSize, block2));
			Assert::AreEqual(block2.Offset, allocateSize);
			Assert::AreEqual(block2.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize * 2);

			FreelistAllocatedBlock block3;
			Assert::IsTrue(list.Allocate(allocateSize, block3));
			Assert::AreEqual(block3.Offset, allocateSize * 2);
			Assert::AreEqual(block3.Size, allocateSize);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize * 3);

			list.Return(block2);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize * 2);

			list.Return(block1);
			Assert::AreEqual(list.GetFreeSpace(), size - allocateSize);

			list.Return(block3);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(AllocateAndFreeVaryingSizeEntries)
		{
			constexpr uint64_t size = 512;
			Freelist list(size);

			constexpr uint64_t block1Size = 64;
			FreelistAllocatedBlock block1;
			Assert::IsTrue(list.Allocate(block1Size, block1));
			Assert::AreEqual(block1.Offset, static_cast<uint64_t>(0));
			Assert::AreEqual(block1.Size, block1Size);

			constexpr uint64_t block2Size = 128;
			FreelistAllocatedBlock block2;
			Assert::IsTrue(list.Allocate(block2Size, block2));
			Assert::AreEqual(block2.Offset, block1Size);
			Assert::AreEqual(block2.Size, block2Size);

			constexpr uint64_t block3Size = 256;
			FreelistAllocatedBlock block3;
			Assert::IsTrue(list.Allocate(block3Size, block3));
			Assert::AreEqual(block3.Offset, block1Size + block2Size);
			Assert::AreEqual(block3.Size, block3Size);

			list.Return(block1);
			list.Return(block2);
			Assert::AreEqual(list.GetFreeSpace(), size - block3Size);

			constexpr uint64_t block4Size = block1Size + block2Size;
			FreelistAllocatedBlock block4;
			Assert::IsTrue(list.Allocate(block4Size, block4));
			Assert::AreEqual(block4.Offset, static_cast<uint64_t>(0));
			Assert::AreEqual(block4.Size, block4Size);

			constexpr uint64_t block5Size = size;
			FreelistAllocatedBlock block5;
			Assert::IsFalse(list.Allocate(block5Size, block5));

			list.Return(block3);
			list.Return(block4);
			Assert::AreEqual(list.GetFreeSpace(), size);
		}

		TEST_METHOD(Resize)
		{
			Freelist list(64);

			FreelistAllocatedBlock block1;
			Assert::IsTrue(list.Allocate(32, block1));

			list.Resize(128);
			Assert::IsTrue(list.GetFreeSpace() == 96);

			FreelistAllocatedBlock block2;
			Assert::IsTrue(list.Allocate(64, block2));
			Assert::IsTrue(list.GetFreeSpace() == 32);

			list.Resize(96);
			Assert::IsTrue(list.GetFreeSpace() == 0);
		}
	};
}
