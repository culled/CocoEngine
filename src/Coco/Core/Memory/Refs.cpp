//
// Created by cullen on 2/28/26.
//

#include "Refs.h"

namespace Coco
{
    RefControlBlock::RefControlBlock(Allocator& alloc, void* ptr, uint32 ptrSize) noexcept :
        Alloc(&alloc),
        Ptr(ptr),
        PtrSize(ptrSize),
        WeakUseCount(0)
    {}

    void FreeControlBlock(RefControlBlock* controlPtr) noexcept
    {
        Allocator* alloc = controlPtr->Alloc;
        uint64 size = controlPtr->PtrSize;
        Destruct(controlPtr);
        alloc->Free(controlPtr, size);
    }
}
