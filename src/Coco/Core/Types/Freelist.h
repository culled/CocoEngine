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
	};

	/// @brief A list for tracking allocations of memory blocks
	class COCOAPI Freelist
	{
	private:
		std::forward_list<FreelistNode> _freeNodes;
		uint64_t _size = 0;

	public:
		Freelist(uint64_t size)
		{
			AllocateMemory(size);
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
			if (it->Size - requiredSize >= 0)
			{
				_freeNodes.emplace_after(it, it->Offset + requiredSize, it->Size - requiredSize);
				block.Size = requiredSize;
			}
			else
			{
				block.Size = it->Size;
			}

			block.Offset = it->Offset;

			// Erase after the previous iterator
			_freeNodes.erase_after(beforeIt);

			return true;
		}

		/// @brief Frees a block of memory in this freelist
		/// @param block The block to free
		void Return(const FreelistAllocatedBlock& block)
		{
			if (block.Offset >= _size)
				return;

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
			_freeNodes.emplace_front(0, _size);
		}

		/// @brief Gets the amount of free memory in this Freelist
		/// @return The number of free bytes in this list's memory
		uint64_t GetFreeSpace() const
		{
			uint64_t freeSpace = 0;

			for (const FreelistNode& node : _freeNodes)
				freeSpace += node.Size;

			return freeSpace;
		}

		/// @brief Gets the size of this freelist
		/// @return The size
		uint64_t GetSize() const { return _size; }

		/// @brief Resizes this freelist, copying memory to the new, resized buffer
		/// @param newSize The new size of this freelist
		void Resize(uint64_t newSize)
		{
			if (newSize == _size)
				return;

			AllocateMemory(newSize);
		}

		private:
			/// @brief Allocates new memory for this freelist, copying old data and adjusting the free nodes
			/// @param size The new size of the freelist
			void AllocateMemory(uint64_t size)
			{
				uint64_t oldSize = _size;
				_size = size;

				// Add a free block if we previously had no free space and now have more memory
				if (_freeNodes.empty() && _size > oldSize)
				{
					_freeNodes.emplace_front(oldSize, _size - oldSize);
				}
				else
				{
					auto beforeIt = _freeNodes.before_begin();
					for (auto it = _freeNodes.begin(); it != _freeNodes.end(); it++)
					{

						if (it->Offset + it->Size > _size)
						{
							// Trim the block if its end extends past the new size
							it->Size = _size - it->Offset;
						}
						else if (it->Offset + it->Size == oldSize && _size > oldSize)
						{
							// Extend the block if it was at the end of the old size and the list is bigger now
							it->Size += _size - oldSize;
						}

						// Erase free blocks past the new size
						while (it != _freeNodes.end() && (it->Offset >= _size || it->Size <= 0))
						{
							it = _freeNodes.erase_after(beforeIt);
						}

						if (it == _freeNodes.end())
							break;

						beforeIt = it;
					}
				}
			}
	};
}