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

    template <typename ValueType>
    class ArrayContainerSorter
    {
    public:
        using CompareFunc = std::function<bool(const ValueType&, const ValueType&)>;
        using Container = ArrayContainer<ValueType>;

    public:
        virtual ~ArrayContainerSorter() noexcept = default;

        virtual void Sort(Container& container) = 0;

    protected:
        ArrayContainerSorter(const CompareFunc& comparator) :
            _comparator(comparator)
        {}

    protected:
        CompareFunc _comparator;
    };
} // Coco

#endif //COCOENGINE_ARRAYCONTAINERSORTER_H