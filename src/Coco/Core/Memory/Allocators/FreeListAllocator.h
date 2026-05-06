//
// Created by cullen on 4/8/26.
//

#ifndef COCOENGINE_FREELISTALLOCATOR_H
#define COCOENGINE_FREELISTALLOCATOR_H
#include "Coco/Core/Memory/Allocator.h"

namespace Coco
{
    class FreeListAllocator : public Allocator
    {
    public:
        struct FreeListBlock
        {
            uint64 Size;
            FreeListBlock* Next;
            bool IsFree;
        };

        FreeListAllocator(uint8 group, uint64 size, Allocator* baseAllocator = nullptr);
        ~FreeListAllocator() override;

        void* Allocate(uint64 size) override;
        void Free(void* memory, uint64 size) noexcept override;

        uint64 GetUsage() const noexcept;
        uint64 GetSize() const noexcept { return _size;}

    private:
        MemoryManager* _memoryManager;
        Allocator* _baseAllocator;
        uint64 _size;
        void* _data;
        FreeListBlock* _firstBlock;

    private:
        void MergeAdjacentBlocks();
    };
} // Coco

#endif //COCOENGINE_FREELISTALLOCATOR_H