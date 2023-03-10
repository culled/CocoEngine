#include "BufferTypes.h"

namespace Coco::Rendering
{
    BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) noexcept
    {
        return static_cast<BufferUsageFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b) noexcept
    {
        return static_cast<BufferUsageFlags>(static_cast<int>(a) & static_cast<int>(b));
    }

    void operator|=(BufferUsageFlags& a, BufferUsageFlags b) noexcept
    {
        a = a | b;
    }
    
    void operator&=(BufferUsageFlags& a, BufferUsageFlags b) noexcept
    {
        a = a & b;
    }
}
