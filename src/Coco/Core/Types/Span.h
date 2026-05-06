//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_SPAN_H
#define COCOENGINE_SPAN_H
#include <span>

namespace Coco
{
    template<typename ValueType, std::size_t Extent = std::dynamic_extent>
    using Span = std::span<ValueType, Extent>;
}
#endif //COCOENGINE_SPAN_H