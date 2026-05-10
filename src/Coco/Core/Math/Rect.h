//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_RECT_H
#define COCOENGINE_RECT_H
#include "Size.h"
#include "Vector2.h"

namespace Coco
{
    /// @brief Base class for an axis-aligned rectangle
    /// @tparam ValueType The value type
    template<typename ValueType>
    struct BaseRect
    {
        /// @brief The position of the lower-left corner of this rectangle from the origin
        BaseVector2<ValueType> Offset;

        /// @brief The size of this rectangle
        BaseSize<ValueType> Size;

        BaseRect() :
            Offset(),
            Size()
        {}

        BaseRect(const BaseVector2<ValueType>& offset, const BaseSize<ValueType>& size) :
            Offset(offset),
            Size(size)
        {}
    };

    /// @brief An axis-aligned rectangle backed by floats
    using Rect = BaseRect<float>;

    /// @brief An axis-aligned rectangle backed by ints
    using Recti = BaseRect<int>;
} // Coco

#endif //COCOENGINE_RECT_H