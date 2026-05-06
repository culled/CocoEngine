//
// Created by cullen on 4/8/26.
//

#include "FreeListAllocator.h"

#include "Coco/Core/Memory/MemoryManager.h"
#include "Coco/Core/EnginePlatform.h"

namespace Coco
{
    FreeListAllocator::FreeListAllocator(uint8 group, uint64 size, Allocator* baseAllocator) :
        Allocator(group),
        _memoryManager(MemoryManager::Get()),
        _baseAllocator(baseAllocator),
        _size(size),
        _data(nullptr),
        _firstBlock(nullptr)
    {
        if (_baseAllocator)
        {
            _data = _baseAllocator->Allocate(size);
        }
        else
        {
            _data = _memoryManager->GetPlatform()->AllocateMemory(_group, size);
        }

        COCO_ASSERT(_data, "Data could not be allocated");

        _firstBlock = static_cast<FreeListBlock*>(_data);
        _firstBlock->Size = size - sizeof(FreeListBlock);
        _firstBlock->IsFree = true;
        _firstBlock->Next = nullptr;
    }

    FreeListAllocator::~FreeListAllocator()
    {
        _memoryManager->GetPlatform()->FreeMemory(_group, _data, _size);
    }

    void* FreeListAllocator::Allocate(uint64 size)
    {
        FreeListBlock* current = _firstBlock;

        while (current)
        {
            // Find a block whose size is large enough for the data
            if (current->IsFree && current->Size >= size)
            {
                // Figure out the size needed when taking into account alignment of the data
                uint64 dataAddress = reinterpret_cast<uint64>(current) + sizeof(FreeListBlock);
                uint64 dataPadding = Math::GetAlignmentOffset(dataAddress, 8) - dataAddress;
                uint64 totalSize = size + dataPadding;

                if (current->Size >= totalSize)
                {
                    // The block can hold the aligned data
                    current->IsFree = false;

                    uint64 blockRemainingSize = current->Size - totalSize;

                    // Figure out where we would split this block
                    uint64 nextBlockAddress = dataAddress + totalSize;
                    uint64 nextBlockPadding = Math::GetAlignmentOffset(nextBlockAddress, alignof(FreeListBlock)) - nextBlockAddress;

                    // Split the block if we have the space to
                    if (blockRemainingSize >= nextBlockPadding + sizeof(FreeListBlock) + 8)
                    {
                        FreeListBlock* splitBlock = reinterpret_cast<FreeListBlock*>(nextBlockAddress + nextBlockPadding);
                        splitBlock->IsFree = true;
                        splitBlock->Next = current->Next;
                        splitBlock->Size = current->Size - (totalSize + sizeof(FreeListBlock) + nextBlockPadding);

                        current->Next = splitBlock;
                        current->Size = totalSize;
                    }

                    return reinterpret_cast<void*>(dataAddress + dataPadding);
                }
            }

            current = current->Next;
        }

        COCO_ASSERT(false, "Out of memory");

        return nullptr;
    }

    void FreeListAllocator::Free(void* memory, uint64 size) noexcept
    {
        FreeListBlock* current = _firstBlock;

        while (current)
        {
            uint64 memoryAddress = reinterpret_cast<uint64>(memory);
            uint64 blockDataAddress = reinterpret_cast<uint64>(current) + sizeof(FreeListBlock);

            // Find out if the memory address lands within this block
            if (blockDataAddress <= memoryAddress && memoryAddress < blockDataAddress + current->Size)
            {
                current->IsFree = true;

                MergeAdjacentBlocks();
                return;
            }

            current = current->Next;
        }

        COCO_ASSERT(false, "Failed to find memory in FreeList");
    }

    uint64 FreeListAllocator::GetUsage() const noexcept
    {
        uint64 usedBytes = 0;
        FreeListBlock* current = _firstBlock;

        while (current)
        {
            if (!current->IsFree)
                usedBytes += current->Size + sizeof(FreeListBlock);

            current = current->Next;
        }

        return usedBytes;
    }

    void FreeListAllocator::MergeAdjacentBlocks()
    {
        FreeListBlock* current = _firstBlock;

        while (current && current->Next)
        {
            if (current->IsFree && current->Next->IsFree)
            {
                current->Size += sizeof(FreeListBlock) + current->Next->Size;
                current->Next = current->Next->Next;
            }
            else
            {
                current = current->Next;
            }
        }
    }
} // Coco