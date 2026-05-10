//
// Created by cullen on 2/28/26.
//

#ifndef COCOENGINE_OPTIONAL_H
#define COCOENGINE_OPTIONAL_H
#include <optional>

namespace Coco
{
    /// @brief A container that can be used to show if a value exists or not
    /// @tparam ValueType The value type
    template<typename ValueType>
    using Optional = std::optional<ValueType>;
}

#endif //COCOENGINE_OPTIONAL_H