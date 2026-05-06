//
// Created by cullen on 2/26/26.
//

#include "LinearAllocator.h"

#include "../MemoryManager.h"
#include "Coco/Core/EnginePlatform.h"
#include "Coco/Core/Math/Math.h"

namespace Coco
{
    LinearAllocator::LinearAllocator(uint8 group, uint64 size, Allocator* baseAllocator) :
        Allocator(group),
        _memoryManager(MemoryManager::Get()),
        _baseAllocator(baseAllocator),
        _size(size),
        _data(nullptr),
        _currentPtr(nullptr),
        _maxUsage(0),
        _allocationCount(0)
    {
        if (_baseAllocator)
        {
            _data = _baseAllocator->Allocate(size);
        }
        else
        {
            _data = _memoryManager->GetPlatform()->AllocateMemory(_group, size);
        }

        COCO_ASSERT(_data, "Data could not be allocated");

        _currentPtr = _data;
    }

    LinearAllocator::~LinearAllocator() noexcept
    {
        COCO_ASSERT(_allocationCount == 0, "Not all allocations were freed");

        if (_baseAllocator)
        {
            _baseAllocator->Free(_data, _size);
        }
        else
        {
            _memoryManager->GetPlatform()->FreeMemory(_group, _data, _size);
        }
    }

    void* LinearAllocator::Allocate(uint64 size)
    {
        // Find an address in memory that satisfies the alignment
        uint64 newOffset = Math::GetAlignmentOffset(reinterpret_cast<uint64>(_currentPtr), alignof(max_align_t));
        uint64 newSize = (newOffset - reinterpret_cast<uint64>(_data)) + size;
        if (newSize > _size)
            return nullptr;

        // Move the current position forward by the allocated size
        _currentPtr = reinterpret_cast<void*>(newOffset + size);
        ++_allocationCount;

        _maxUsage = Math::Max(_maxUsage, GetUsage());

        return reinterpret_cast<uint8*>(newOffset);
    }

    void LinearAllocator::Free(void* memory, uint64 size) noexcept {}

    void LinearAllocator::Reset() noexcept
    {
        _currentPtr = _data;
        _allocationCount = 0;
    }
} // Coco