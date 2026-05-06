//
// Created by cullen on 3/11/26.
//

#include "BufferTypes.h"

namespace Coco
{
    BufferDescription::BufferDescription(uint64 size, BufferUsageFlags usageFlags) :
        Size(size),
        UsageFlags(usageFlags)
    {}
}
