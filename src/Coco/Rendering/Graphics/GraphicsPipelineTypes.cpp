#include "GraphicsPipelineTypes.h"

namespace Coco::Rendering
{
    uint32_t GetBufferDataFormatSize(BufferDataFormat format)
    {
        switch (format)
        {
        case BufferDataFormat::Float:
        case BufferDataFormat::Int:
            return 32;
        case BufferDataFormat::Vector2:
        case BufferDataFormat::Vector2Int:
            return 32 * 2;
        case BufferDataFormat::Vector3:
        case BufferDataFormat::Vector3Int:
            return 32 * 3;
        case BufferDataFormat::Vector4:
        case BufferDataFormat::Vector4Int:
            return 32 * 4;
        default:
            return 0;
        }
    }
}
