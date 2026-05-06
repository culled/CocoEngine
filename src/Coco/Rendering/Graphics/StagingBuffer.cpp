//
// Created by cullen on 3/28/26.
//

#include "StagingBuffer.h"

namespace Coco
{
    StagingOperation::StagingOperation() :
        StagingBuffer(),
        BufferOffset(0),
        BufferPtr(nullptr),
        Size(0)
    {}

    StagingOperation::StagingOperation(uint64 size) :
        StagingBuffer(),
        BufferOffset(0),
        BufferPtr(nullptr),
        Size(size)
    {}
} // Coco