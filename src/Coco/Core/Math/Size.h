//
// Created by cullen on 2/27/26.
//

#ifndef COCOENGINE_SIZE_H
#define COCOENGINE_SIZE_H
#include <type_traits>

namespace Coco
{
    template<typename ValueType, typename = std::enable_if_t<std::is_arithmetic_v<ValueType>, ValueType>>
    struct BaseSize
    {
        ValueType Width;
        ValueType Height;

        BaseSize() : Width(), Height() {}
        BaseSize(const ValueType& width, const ValueType& height) : Width(width), Height(height) {}

        template<typename ReturnType, typename = std::enable_if_t<std::is_floating_point_v<ReturnType>>>
        ReturnType GetAspectRatio() const
        {
            return static_cast<ReturnType>(Width) / static_cast<ReturnType>(Height);
        }
    };

    template<typename ValueType>
    bool operator==(const BaseSize<ValueType>& a, const BaseSize<ValueType>& b)
    {
        return a.Width == b.Width && a.Height == b.Height;
    }

    template<typename ValueType>
    bool operator!=(const BaseSize<ValueType>& a, const BaseSize<ValueType>& b)
    {
        return !(a == b);
    }

    using Size = BaseSize<float>;
    using Sizei = BaseSize<int>;
} // Coco

#endif //COCOENGINE_SIZE_H