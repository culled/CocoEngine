//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_STACKALLOCATOR_H
#define COCOENGINE_STACKALLOCATOR_H
#include "../Allocator.h"

namespace Coco
{
    class StackAllocator : public Allocator
    {
    public:
        StackAllocator(uint8 group, uint64 size, Allocator* baseAllocator = nullptr);
        ~StackAllocator() noexcept override;

        void* Allocate(uint64 size) override;
        void Free(void* memory, uint64 size) noexcept override;

        uint64 GetMaximumUsage() const { return _maxUsage; }

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