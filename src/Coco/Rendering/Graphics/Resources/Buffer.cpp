//
// Created by cullen on 3/12/26.
//

#include "Buffer.h"

namespace Coco
{
    Buffer::Buffer(uint64 id) : GraphicsResource(id)
    {}

    BufferAllocation::BufferAllocation(Ref<Buffer> buffer, uint64 offset, uint64 size) :
        DataBuffer(buffer),
        BufferOffset(offset),
        Size(size)
    {}
} // Coco