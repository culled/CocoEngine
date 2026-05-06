//
// Created by cullen on 3/11/26.
//

#ifndef COCOENGINE_BUFFERTYPES_H
#define COCOENGINE_BUFFERTYPES_H
#include <Coco/Core/Types/CoreTypes.h>
#include <Coco/Core/Types/EnumTypes.h>

namespace Coco
{
    /// @brief Types of buffer usage
    enum class BufferUsageFlags : uint8
    {
        None = 0,
        TransferSource = 1 << 0,
        TransferDestination = 1 << 1,
        Uniform = 1 << 2,
        Index = 1 << 3,
        Vertex = 1 << 4,
        HostVisible = 1 << 5,
    };

    EnumFlagOperators(BufferUsageFlags)

    struct BufferDescription
    {
        uint64 Size;
        BufferUsageFlags UsageFlags;

        BufferDescription(uint64 size, BufferUsageFlags usageFlags);
    };
}
#endif //COCOENGINE_BUFFERTYPES_H