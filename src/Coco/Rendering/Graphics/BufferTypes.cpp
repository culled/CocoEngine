#include "Renderpch.h"
#include "BufferTypes.h"

namespace Coco::Rendering
{
    uint8 GetDataTypeSize(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::Float:
            return BufferFloatSize;
        case BufferDataType::Float2:
            return BufferFloatSize * 2;
        case BufferDataType::Float3:
            return BufferFloatSize * 3;
        case BufferDataType::Float4:
            return BufferFloatSize * 4;
        case BufferDataType::Mat4x4:
            return BufferFloatSize * 4 * 4;
        case BufferDataType::Int:
            return BufferIntSize;
        case BufferDataType::Int2:
            return BufferIntSize * 2;
        case BufferDataType::Int3:
            return BufferIntSize * 3;
        case BufferDataType::Int4:
            return BufferIntSize * 4;
        case BufferDataType::Bool:
            return 1;
        default:
            break;
        }

        Assert(false)
        return 0;
    }

    uint8 GetDataTypeComponentCount(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::Float:
        case BufferDataType::Int:
        case BufferDataType::Bool:
            return 1;
        case BufferDataType::Float2:
        case BufferDataType::Int2:
            return 2;
        case BufferDataType::Float3:
        case BufferDataType::Int3:
            return 3;
        case BufferDataType::Float4:
        case BufferDataType::Int4:
            return 4;
        case BufferDataType::Mat4x4:
            return 4 * 4;
        default:
            break;
        }

        Assert(false)
        return 0;
    }
}