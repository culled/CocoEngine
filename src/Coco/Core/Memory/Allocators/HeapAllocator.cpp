//
// Created by cullen on 2/24/26.
//

#include "HeapAllocator.h"
#include "../MemoryManager.h"
#include "Coco/Core/EnginePlatform.h"

namespace Coco
{
    HeapAllocator::HeapAllocator(uint8 group) noexcept :
        Allocator(group)
    {}

    void* HeapAllocator::Allocate(uint64 size)
    {
        auto memoryManager = MemoryManager::Get();
        COCO_ASSERT(memoryManager, "MemoryManager singleton was null");

        auto memory = memoryManager->GetPlatform()->AllocateMemory(_group, size);
        COCO_ASSERT(memory, "Data could not be allocated");

        return memory;
    }

    void HeapAllocator::Free(void* memory, uint64 size) noexcept
    {
        auto memoryManager = MemoryManager::Get();
        COCO_ASSERT(memoryManager, "MemoryManager singleton was null");

        memoryManager->GetPlatform()->FreeMemory(_group, memory, size);
    }
} // Coco
