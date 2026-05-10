//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_ARRAYCONTAINERSORTER_H
#define COCOENGINE_ARRAYCONTAINERSORTER_H
#include "ArrayContainer.h"
#include <functional>

namespace Coco
{
    template<typename>
    class ArrayContainer;

    /// @brief Base class for a class that can sort elements of an array
    /// @tparam ValueType The type of array element
    template <typename ValueType>
    class ArrayContainerSorter
    {
    public:
        using CompareFunc = std::function<bool(const ValueType&, const ValueType&)>;
        using Container = ArrayContainer<ValueType>;

        virtual ~ArrayContainerSorter() noexcept = default;

        /// @brief Sorts the given array
        /// @param container The array to sort
        virtual void Sort(Container& container) = 0;

    protected:
        ArrayContainerSorter(const CompareFunc& comparator) :
            _comparator(comparator)
        {}

        CompareFunc _comparator;
    };
} // Coco

#endif //COCOENGINE_ARRAYCONTAINERSORTER_H