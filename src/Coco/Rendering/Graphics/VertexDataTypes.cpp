//
// Created by cullen on 3/14/26.
//
#include "VertexDataTypes.h"

namespace Coco
{
    uint8 GetVertexChannelElementCount(VertexChannel channel)
    {
        switch (channel)
        {
            case VertexChannel::Position:
            case VertexChannel::Normal:
                return 3;
            case VertexChannel::Color:
            case VertexChannel::Tangent:
                return 4;
            case VertexChannel::UV0:
                return 2;
            default:
                return 0;
        }
    }
}
