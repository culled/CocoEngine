//
// Created by cullen on 3/22/26.
//

#include "StringContainer.h"
#include "Exception.h"

namespace Coco::Detail
{
    void ThrowOutOfRangeException(const char* messageFormat, uint64 index, uint64 size)
    {
        throw Exception(FormatString(messageFormat, index, size));
    }
}
