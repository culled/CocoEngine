//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_ALLOCATOR_H
#define COCOENGINE_ALLOCATOR_H
#include "Coco/Core/Types/CoreTypes.h"

namespace Coco
{
    class MemoryManager;

    /// @brief Base class for objects that define memory allocation strategies
    class Allocator
    {
    public:
        virtual ~Allocator() noexcept = default;

        /// @brief Gets the default allocator for the EnginePlatform
        /// @return The allocator
        static Allocator* GetDefaultAllocator();

        /// @brief Allocates a block of memory
        /// @param size The number of bytes to allocate
        /// @return A pointer to the block of allocated memory
        virtual void* Allocate(uint64 size) = 0;

        /// @brief Frees a block of allocated memory.
        /// NOTE: Memory must be freed by the Allocator that allocated it!
        /// @param memory A pointer to the block of allocated memory
        /// @param size The size of the memory, in bytes
        virtual void Free(void* memory, uint64 size) noexcept = 0;

        /// @brief Gets this Allocator's group, which is used to group different types of allocations
        /// @return The group
        uint8 GetGroup() const noexcept { return _group; }

    protected:
        Allocator(uint8 group) noexcept;

    protected:
        uint8 _group;
    };
} // Coco

#endif //COCOENGINE_ALLOCATOR_H
