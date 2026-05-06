//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_MEMORYMANAGER_H
#define COCOENGINE_MEMORYMANAGER_H

#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Types/Map.h"

namespace Coco
{
    class EnginePlatform;

    /// @brief A Singleton that tracks memory allocations made for the Engine
    class MemoryManager
    {
    public:
        /// @brief Information for an allocation group
        struct AllocationGroupInfo
        {
            /// @brief The number of bytes allocated for this group
            uint64 BytesAllocated;

            /// @brief The number of allocations made within this group
            uint64 AllocationCount;
        };

    public:
        MemoryManager(EnginePlatform& platform) noexcept;
        ~MemoryManager() noexcept;

        static MemoryManager* Get() noexcept { return _singleton; }

        /// @brief Records an allocation for a group
        /// @param group The allocation group
        /// @param bytesAllocated The number of bytes allocated
        void AllocationMade(uint8 group, uint64 bytesAllocated) noexcept;

        /// @brief Records a freed allocation for a group
        /// @param group The allocation group
        /// @param bytesFreed The number of bytes freed
        void AllocationFreed(uint8 group, uint64 bytesFreed) noexcept;

        /// @brief Gets the EnginePlatform that this MemoryManager is tied to
        /// @return The EnginePlatform
        EnginePlatform* GetPlatform() const noexcept { return _platform; }

        /// @brief Gets the total number of bytes that have been allocated by all Allocators
        /// @return The total number of bytes that have been allocated
        uint64 GetTotalUsage() const noexcept;

    private:
        static MemoryManager* _singleton;

        EnginePlatform* _platform;
        std::array<AllocationGroupInfo, 255> _allocationGroups;
    };
} // Coco

#endif //COCOENGINE_MEMORYMANAGER_H
