//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_RECT_H
#define COCOENGINE_RECT_H
#include "Size.h"
#include "Vector2.h"

namespace Coco
{
    template<typename ValueType>
    struct BaseRect
    {
        BaseVector2<ValueType> Offset;
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

    using Rect = BaseRect<float>;
    using Recti = BaseRect<int>;
} // Coco

#endif //COCOENGINE_RECT_H