#pragma once

#include <Coco/Core/Core.h>
#include <forward_list>
#include <Coco/Core/Types/Array.h>

namespace Coco
{
	/// @brief A free block of memory within a Freelist
	struct FreelistNode
	{
		/// @brief The offset of the block's start
		uint64_t Offset;

		/// @brief The size of the free block
		uint64_t Size;

		FreelistNode() :
			Offset(0), Size(0)
		{}

		FreelistNode(uint64_t offset, uint64_t size) :
			Offset(offset), Size(size)
		{}
	};

	/// @brief An allocated block from a Freelist
	struct FreelistAllocatedBlock
	{
		/// @brief The offset within the Freelist
		uint64_t Offset = 0;

		/// @brief The size of the allocated block
		uint64_t Size = 0;

		/// @brief A pointer to the first byte of this block's memory
		void* Memory = nullptr;
	};

	/// @brief An list for allocating and returning blocks of its memory
	/// @tparam Size The size of the memory that can be allocated
	template<uint64_t Size>
	class COCOAPI Freelist
	{
	private:
		static constexpr uint64_t _minSplitSize = 32;

		std::forward_list<FreelistNode> _freeNodes;
		Array<char, Size> _data;

	public:
		Freelist() :
			_data{}
		{
			_freeNodes.emplace_front(0, Size);
		}
		virtual ~Freelist() = default;

		/// @brief Tries to allocate a block of memory from this freelist
		/// @param requiredSize The required size of memory
		/// @param block The block that will be filled with the memory information
		/// @return True if a block was allocated successfully
		bool Allocate(uint64_t requiredSize, FreelistAllocatedBlock& block)
		{
			std::forward_list<FreelistNode>::iterator beforeIt = _freeNodes.before_begin();
			std::forward_list<FreelistNode>::iterator it;

			// Find the first free node that is big enough for the required size
			for (it = _freeNodes.begin(); it != _freeNodes.end(); it++)
			{
				if (it->Size >= requiredSize)
					break;

				beforeIt = it;
			}

			if (it == _freeNodes.end())
			{
				// No free blocks large enough for the requested memory
				return false;
			}

			// Split the free node if there will be leftover memory
			if (it->Size - requiredSize > _minSplitSize)
			{
				_freeNodes.emplace_after(it, it->Offset + requiredSize, it->Size - requiredSize);
				block.Size = requiredSize;
			}
			else
			{
				block.Size = it->Size;
			}

			block.Offset = it->Offset;
			block.Memory = _data.data() + block.Offset;

			// Erase after the previous iterator
			_freeNodes.erase_after(beforeIt);

			return true;
		}

		/// @brief Frees a block of memory in this freelist
		/// @param block The block to free
		void Return(const FreelistAllocatedBlock& block)
		{
			std::forward_list<FreelistNode>::iterator beforeIt = _freeNodes.before_begin();
			std::forward_list<FreelistNode>::iterator afterIt;

			// Find the free nodes before and after the free space
			for (afterIt = _freeNodes.begin(); afterIt != _freeNodes.end(); afterIt++)
			{
				if (afterIt->Offset > block.Offset)
					break;

				beforeIt = afterIt;
			}

			std::forward_list<FreelistNode>::iterator it = _freeNodes.insert_after(beforeIt, FreelistNode(block.Offset, block.Size));

			// Try to combine the node before with the new one
			if (it != _freeNodes.begin() && beforeIt->Offset + beforeIt->Size == it->Offset)
			{
				beforeIt->Size += it->Size;
				_freeNodes.erase_after(beforeIt);
				it = beforeIt;
			}

			// Try to combine the new node with the one after it
			if (afterIt != _freeNodes.end() && it->Offset + it->Size == afterIt->Offset)
			{
				it->Size += afterIt->Size;
				_freeNodes.erase_after(it);
			}
		}

		/// @brief Clears and resets this Freelist
		void Clear()
		{
			_freeNodes.clear();
			_freeNodes.emplace_front(0, Size);
		}

		/// @brief Gets the amount of free memory in this Freelist
		/// @return The number of free bytes in this list's memory
		uint64_t GetFreeSpace()
		{
			uint64_t freeSpace = 0;

			for (const FreelistNode& node : _freeNodes)
				freeSpace += node.Size;

			return freeSpace;
		}
	};
}