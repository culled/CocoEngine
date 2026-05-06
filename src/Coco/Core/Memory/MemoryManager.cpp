//
// Created by cullen on 2/24/26.
//

#include "MemoryManager.h"

#include "Coco/Core/Asserts.h"
#include "Coco/Core/EnginePlatform.h"

namespace Coco
{
    MemoryManager* MemoryManager::_singleton = nullptr;

    MemoryManager::MemoryManager(EnginePlatform& platform) noexcept :
        _platform(&platform),
        _allocationGroups{}
    {
        _singleton = this;
    }

    MemoryManager::~MemoryManager() noexcept
    {
        _singleton = nullptr;
    }

    void MemoryManager::AllocationMade(uint8 group, uint64 bytesAllocated) noexcept
    {
        AllocationGroupInfo& groupInfo = _allocationGroups[group];
        groupInfo.AllocationCount++;
        groupInfo.BytesAllocated += bytesAllocated;
    }

    void MemoryManager::AllocationFreed(uint8 group, uint64 bytesFreed) noexcept
    {
        auto& groupInfo = _allocationGroups[group];
        COCO_ASSERT(groupInfo.AllocationCount > 0, "AllocationCount for group %u must be more than 0", group);
        COCO_ASSERT(groupInfo.BytesAllocated >= bytesFreed, "BytesAllocated for group %u must be >= %u", group, bytesFreed);

        groupInfo.AllocationCount--;
        groupInfo.BytesAllocated -= bytesFreed;

        // Sanity check for making sure all bytes freed match all bytes allocated
        COCO_ASSERT(groupInfo.AllocationCount > 0 || groupInfo.BytesAllocated == 0, "Memory freed in group %u did not equal memory allocated. Remaining bytes: %u", group, groupInfo.BytesAllocated);
    }

    uint64 MemoryManager::GetTotalUsage() const noexcept
    {
        uint64 total = 0;
        for (const auto& groupInfo : _allocationGroups)
            total += groupInfo.BytesAllocated;

        return total;
    }
} // Coco
