#include "Corepch.h"
#include "Freelist.h"

namespace Coco
{
	FreelistNode::FreelistNode() : 
		FreelistNode(0, 0)
	{}

	FreelistNode::FreelistNode(uint64 offset, uint64 size) :
		Offset(offset),
		Size(size)
	{}

	Freelist::Freelist(uint64 size) :
		_size(0)
	{
		AllocateMemory(size);
	}

	Freelist::~Freelist()
	{
		_freeNodes.clear();
	}

	bool Freelist::Allocate(uint64 requiredSize, FreelistNode& block)
	{
		ListIterator beforeIt = _freeNodes.before_begin();
		ListIterator it;

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

	void Freelist::Free(const FreelistNode& block)
	{
		if (block.Offset >= _size)
			return;

		ListIterator beforeIt = _freeNodes.before_begin();
		ListIterator afterIt;

		// Find the free nodes before and after the free space
		for (afterIt = _freeNodes.begin(); afterIt != _freeNodes.end(); afterIt++)
		{
			if (afterIt->Offset > block.Offset)
				break;

			beforeIt = afterIt;
		}

		ListIterator it = _freeNodes.insert_after(beforeIt, FreelistNode(block.Offset, block.Size));

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

	void Freelist::Clear()
	{
		_freeNodes.clear();
		_freeNodes.emplace_front(0, _size);
	}

	uint64 Freelist::GetFreeSpace() const
	{
		uint64 freeSpace = 0;

		for (const FreelistNode& node : _freeNodes)
			freeSpace += node.Size;

		return freeSpace;
	}

	void Freelist::Resize(uint64 newSize)
	{
		if (newSize == _size)
			return;

		AllocateMemory(newSize);
	}

	void Freelist::AllocateMemory(uint64 size)
	{
		uint64 oldSize = _size;
		_size = size;

		// Add a free block if we previously had no free space and now have more memory
		if (_freeNodes.empty() && _size > oldSize)
		{
			_freeNodes.emplace_front(oldSize, _size - oldSize);
		}
		else
		{
			ListIterator beforeIt = _freeNodes.before_begin();
			for (ListIterator it = _freeNodes.begin(); it != _freeNodes.end(); it++)
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
}