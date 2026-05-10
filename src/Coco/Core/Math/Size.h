//
// Created by cullen on 2/27/26.
//

#ifndef COCOENGINE_SIZE_H
#define COCOENGINE_SIZE_H
#include <type_traits>

namespace Coco
{
    /// @brief Base class for a two-dimensional size with a width and height
    /// @tparam ValueType The value type
    template<typename ValueType, typename = std::enable_if_t<std::is_arithmetic_v<ValueType>, ValueType>>
    struct BaseSize
    {
        /// @brief The width
        ValueType Width;

        /// @brief The height
        ValueType Height;

        BaseSize() : Width(), Height() {}
        BaseSize(const ValueType& width, const ValueType& height) : Width(width), Height(height) {}

        /// @brief Calculates the aspect ratio (width/height) of this size
        /// @tparam ReturnType The return type
        /// @return The aspect ratio
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

    /// @brief A two-dimensional size backed by floats
    using Size = BaseSize<float>;

    /// @brief A two-dimensional size backed by ints
    using Sizei = BaseSize<int>;
} // Coco

#endif //COCOENGINE_SIZE_H