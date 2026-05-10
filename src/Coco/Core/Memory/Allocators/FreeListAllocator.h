//
// Created by cullen on 4/8/26.
//

#ifndef COCOENGINE_FREELISTALLOCATOR_H
#define COCOENGINE_FREELISTALLOCATOR_H
#include "Coco/Core/Memory/Allocator.h"

namespace Coco
{
    /// @brief An allocator that can freely allocate and free chunks of its memory using a linked list to track allocations
    class FreeListAllocator : public Allocator
    {
    public:
        FreeListAllocator(uint8 group, uint64 size, Allocator* baseAllocator = nullptr);
        ~FreeListAllocator() override;

        void* Allocate(uint64 size) override;
        void Free(void* memory, uint64 size) noexcept override;

        /// @brief Gets the number of bytes allocated from this free list
        /// @return The number of bytes currently in use
        uint64 GetUsage() const noexcept;

        /// @brief Gets the total size of this free list
        /// @return The total size of the free list
        uint64 GetSize() const noexcept { return _size; }

    private:
        /// @brief A block within a free list
        struct FreeListBlock
        {
            /// @brief The size of the block, not including the block's header
            uint64 Size;

            /// @brief A pointer to the next block, if one exists
            FreeListBlock* Next;

            /// @brief If true, this block is free and can be allocated from
            bool IsFree;
        };

        MemoryManager* _memoryManager;
        Allocator* _baseAllocator;
        uint64 _size;
        void* _data;
        FreeListBlock* _firstBlock;

        /// @brief Merges adjacent free blocks
        void MergeAdjacentBlocks();
    };
} // Coco

#endif //COCOENGINE_FREELISTALLOCATOR_H