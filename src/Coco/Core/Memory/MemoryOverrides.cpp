//
// Created by cullen on 2/24/26.
//

#include "MemoryOverrides.h"
#include "Allocator.h"

namespace Coco::Memory
{
    void* Allocate(Allocator& allocator, uint64 size)
    {
        return allocator.Allocate(size);
    }

    void Free(Allocator& allocator, void* ptr, uint64 size) noexcept
    {
        allocator.Free(ptr, size);
    }
}
