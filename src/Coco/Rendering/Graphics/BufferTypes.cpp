#include "BufferTypes.h"

namespace Coco::Rendering
{
    BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b)
    {
        return static_cast<BufferUsageFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b)
    {
        return static_cast<BufferUsageFlags>(static_cast<int>(a) & static_cast<int>(b));
    }

    void operator|=(BufferUsageFlags& a, BufferUsageFlags b)
    {
        a = a | b;
    }
    
    void operator&=(BufferUsageFlags& a, BufferUsageFlags b)
    {
        a = a & b;
    }

    uint32_t GetBufferDataFormatSize(BufferDataFormat format)
    {
        switch (format)
        {
        case BufferDataFormat::Float:
        case BufferDataFormat::Int:
            return 4;
        case BufferDataFormat::Vector2:
        case BufferDataFormat::Vector2Int:
            return 4 * 2;
        case BufferDataFormat::Vector3:
        case BufferDataFormat::Vector3Int:
            return 4 * 3;
        case BufferDataFormat::Vector4:
        case BufferDataFormat::Vector4Int:
            return 4 * 4;
        default:
            return 0;
        }
    }
}
