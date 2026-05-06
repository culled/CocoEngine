//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_HEAPALLOCATOR_H
#define COCOENGINE_HEAPALLOCATOR_H
#include "Coco/Core/Memory/Allocator.h"

namespace Coco
{
    /// @brief An Allocator that allocates heap memory from the EnginePlatform
    class HeapAllocator :
            public Allocator
    {
    public:
        HeapAllocator(uint8 group) noexcept;

        void* Allocate(uint64 size) override;
        void Free(void* memory, uint64 size) noexcept override;
    };
} // Coco

#endif //COCOENGINE_HEAPALLOCATOR_H
