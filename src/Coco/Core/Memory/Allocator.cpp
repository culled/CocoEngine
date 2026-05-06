//
// Created by cullen on 2/24/26.
//

#include "Allocator.h"

#include "MemoryManager.h"
#include "Coco/Core/Asserts.h"
#include "../EnginePlatform.h"

namespace Coco
{
    Allocator::Allocator(uint8 group) noexcept :
        _group(group)
    {}

    Allocator* Allocator::GetDefaultAllocator()
    {
        auto memoryManager = MemoryManager::Get();
        COCO_ASSERT(memoryManager, "MemoryManager singleton was null");
        return memoryManager->GetPlatform()->GetDefaultAllocator();
    }
} // Coco
