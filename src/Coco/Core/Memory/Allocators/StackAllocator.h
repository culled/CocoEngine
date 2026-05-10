//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_STACKALLOCATOR_H
#define COCOENGINE_STACKALLOCATOR_H
#include "../Allocator.h"

namespace Coco
{
    /// @brief An allocator that allocates memory onto a stack on a first-in, last-out basis. Only the most recent allocation can be freed at a time
    class StackAllocator : public Allocator
    {
    public:
        StackAllocator(uint8 group, uint64 size, Allocator* baseAllocator = nullptr);
        ~StackAllocator() noexcept override;

        void* Allocate(uint64 size) override;
        void Free(void* memory, uint64 size) noexcept override;

        /// @brief Gets the maximum number of bytes that have been allocated
        /// @return The maximum amount of bytes that have been used by this allocator
        uint64 GetMaximumUsage() const noexcept { return _maxUsage; }

        /// @brief Gets the number of bytes currently used
        /// @return The number of bytes used
        uint64 GetUsage() const noexcept { return _currentOffset; }

    private:
        /// @brief Information for a stack allocation
        struct StackAllocationHeader
        {
            StackAllocationHeader* PreviousHeader;
        };

        MemoryManager* _memoryManager;
        Allocator* _baseAllocator;
        uint64 _size;
        void* _data;
        StackAllocationHeader* _lastHeader;
        uint64 _currentOffset;
        uint64 _maxUsage;
    };
} // Coco

#endif //COCOENGINE_STACKALLOCATOR_H