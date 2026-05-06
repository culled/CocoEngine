//
// Created by cullen on 2/26/26.
//

#include "Math.h"

namespace Coco
{
    uint64 Math::GetAlignmentOffset(uint64 address, uint64 alignment) noexcept
    {
        return alignment > 0 ? (address + alignment - 1) & ~(alignment - 1) : alignment;
    }
} // Coco