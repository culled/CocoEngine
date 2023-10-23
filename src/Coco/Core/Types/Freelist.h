#pragma once

#include "../Corepch.h"
#include "../Defines.h"

namespace Coco
{
	/// @brief A block of memory within a Freelist
	struct FreelistNode
	{
		/// @brief The offset of the block's start
		uint64 Offset;

		/// @brief The size of the block
		uint64 Size;

		FreelistNode();
		FreelistNode(uint64 offset, uint64 size);
	};

	/// @brief Tracks allocations within a block of memory
	class Freelist
	{
	private:
		using ListIterator = std::forward_list<FreelistNode>::iterator;
		std::forward_list<FreelistNode> _freeNodes;
		uint64 _size;

	public:
		Freelist(uint64 size);
		~Freelist();

		/// @brief Tries to allocate a block of memory from this freelist
		/// @param requiredSize The required size of memory
		/// @param outBlock Will be filled with the memory information if successful
		/// @return True if a block was allocated successfully
		bool Allocate(uint64 requiredSize, FreelistNode& outBlock);

		/// @brief Frees a block of memory in this freelist
		/// @param block The block to free
		void Free(const FreelistNode& block);

		/// @brief Clears and resets this Freelist
		void Clear();

		/// @brief Gets the amount of free memory in this Freelist
		/// @return The number of free bytes in this list's memory
		uint64 GetFreeSpace() const;

		/// @brief Gets the size of this freelist
		/// @return The size
		uint64 GetSize() const { return _size; }

		/// @brief Resizes this freelist. Blocks are preserved as long as they're not truncated
		/// @param newSize The new size of this freelist
		void Resize(uint64 newSize);

	private:
		/// @brief Allocates new memory for this freelist, copying old data and adjusting the free nodes
		/// @param size The new size of the freelist
		void AllocateMemory(uint64 size);
	};
}