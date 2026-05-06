//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_OPTIONAL_H
#define COCOENGINE_OPTIONAL_H
#include <optional>

namespace Coco
{
    template<typename ValueType>
    using Optional = std::optional<ValueType>;
}

#endif //COCOENGINE_OPTIONAL_H