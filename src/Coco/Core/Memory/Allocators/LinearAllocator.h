//
// Created by cullen on 2/26/26.
//

#ifndef COCOENGINE_LINEARALLOCATOR_H
#define COCOENGINE_LINEARALLOCATOR_H
#include "../Allocator.h"

namespace Coco
{
    /// @brief An allocator that quickly allocates memory linearly and can only be freed by resetting it, which quickly frees all memory at once
    class LinearAllocator : public Allocator
    {
    public:
        LinearAllocator(uint8 group, uint64 size, Allocator* baseAllocator = nullptr);
        ~LinearAllocator() noexcept override;

        void* Allocate(uint64 size) override;
        void Free(void* memory, uint64 size) noexcept override;

        /// @brief Frees all allocated memory
        void Reset() noexcept;

        /// @brief Gets the number of bytes currently used
        /// @return The number of bytes used
        uint64 GetUsage() const noexcept { return reinterpret_cast<uint64>(_currentPtr) - reinterpret_cast<uint64>(_data); }

        /// @brief Gets the maximum number of bytes that have been used
        /// @return The maximum number of bytes used
        uint64 GetMaximumUsage() const noexcept { return _maxUsage; }

        /// @brief Gets the number of allocations that have been made
        /// @return The number of allocations
        uint64 GetAllocationCount() const noexcept { return _allocationCount; }

        /// @brief Gets the total size of the memory block
        /// @return The block size, in bytes
        uint64 GetSize() const noexcept { return _size; }

        /// @brief Gets the remaining number of bytes that haven't been allocated
        /// @return The number of unallocated bytes
        uint64 GetRemainingSpace() const noexcept { return _size - GetUsage(); }

    private:
        MemoryManager* _memoryManager;
        Allocator* _baseAllocator;
        uint64 _size;
        uint64 _allocationCount;
        uint64 _maxUsage;
        void* _data;
        void* _currentPtr;
    };
} // Coco

#endif //COCOENGINE_LINEARALLOCATOR_H