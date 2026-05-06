//
// Created by cullen on 2/26/26.
//

#include "StackAllocator.h"

#include "Coco/Core/Asserts.h"
#include "../MemoryManager.h"
#include "Coco/Core/EnginePlatform.h"
#include "Coco/Core/Math/Math.h"

namespace Coco
{
    StackAllocator::StackAllocator(uint8 group, uint64 size, Allocator* baseAllocator) :
        Allocator(group),
        _memoryManager(MemoryManager::Get()),
        _baseAllocator(baseAllocator),
        _size(size),
        _data(nullptr),
        _lastHeader(nullptr),
        _currentOffset(0),
        _maxUsage(0)
    {
        if (_baseAllocator)
        {
            _data = _baseAllocator->Allocate(size);
        }
        else
        {
            _data = _memoryManager->GetPlatform()->AllocateMemory(_group, _size);
        }

        COCO_ASSERT(_data, "Data could not be allocated");
    }

    StackAllocator::~StackAllocator() noexcept
    {
        COCO_ASSERT(_currentOffset == 0, "Not all allocations were freed");

        if (_baseAllocator)
        {
            _baseAllocator->Free(_data, _size);
        }
        else
        {
            _memoryManager->GetPlatform()->FreeMemory(_group, _data, _size);
        }
    }

    void* StackAllocator::Allocate(uint64 size)
    {
        uint64 maxAlignment = alignof(StackAllocationHeader);
        uint64 padding = Math::GetAlignmentOffset(_currentOffset, maxAlignment) - _currentOffset;
        auto headerPtr = reinterpret_cast<StackAllocationHeader*>(static_cast<uint8*>(_data) + padding + _currentOffset);

        uint64 allocationSize = padding + sizeof(StackAllocationHeader) + size;
        uint64 remainingSize = _size - _currentOffset;

        COCO_ASSERT(allocationSize <= remainingSize, "StackAllocator is out of memory: requested %u bytes above the stack size of %u bytes", allocationSize - remainingSize, _size);

        StackAllocationHeader* oldHeader = _lastHeader;

        Construct(headerPtr, oldHeader);
        _lastHeader = headerPtr;

        _currentOffset += allocationSize;
        _maxUsage = Math::Max(_maxUsage, _currentOffset);

        return reinterpret_cast<void*>(reinterpret_cast<uint64>(headerPtr) + sizeof(StackAllocationHeader));
    }

    void StackAllocator::Free(void* memory, uint64 size) noexcept
    {
        COCO_ASSERT(_lastHeader, "No allocations have been made in the stack");

        if (reinterpret_cast<uint64>(memory) != reinterpret_cast<uint64>(_lastHeader) + sizeof(StackAllocationHeader))
        {
            COCO_ASSERT(false, "The memory was not at the top of the stack");
            return;
        }

        _currentOffset = reinterpret_cast<uint64>(_lastHeader) - reinterpret_cast<uint64>(_data);
        _lastHeader = _lastHeader->PreviousHeader;
    }
} // Coco