//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_VERTEXDATATYPES_H
#define COCOENGINE_VERTEXDATATYPES_H
#include <Coco/Core/Types/CoreTypes.h>
#include <Coco/Core/Types/EnumTypes.h>

namespace Coco
{
    /// @brief The maximum number of vertex channels
    constexpr uint8 MaxVertexChannelCount = 5;

    /// @brief Channels of vertex data
    enum class VertexChannel : uint8
    {
        Position = 0,
        Normal,
        Tangent,
        Color,
        UV0
    };

    /// @brief Channels of vertex data
    enum class VertexChannelFlags : uint8
    {
        None = 0,
        Position = 1 << 0,
        Normal = 1 << 1,
        Tangent = 1 << 2,
        Color = 1 << 3,
        UV0 = 1 << 4,
    };

    EnumFlagOperators(VertexChannelFlags)

    uint8 GetVertexChannelElementCount(VertexChannel channel);
}

#endif //COCOENGINE_VERTEXDATATYPES_H