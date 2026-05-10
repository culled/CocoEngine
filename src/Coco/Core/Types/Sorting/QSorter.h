//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_QSORTER_H
#define COCOENGINE_QSORTER_H
#include <cmath>

#include "../ArrayContainerSorter.h"

namespace Coco
{
    /// @brief Implementation of sorting using the QuickSort (QSort) algorithm
    /// @tparam ValueType The value type
    template <typename ValueType>
    class QSorter :
        public ArrayContainerSorter<ValueType>
    {
    public:
        using CompareFunc = ArrayContainerSorter<ValueType>::CompareFunc;
        using Container = ArrayContainerSorter<ValueType>::Container;

        QSorter(const CompareFunc& comparator, float pivotFactor = 1.0) :
            ArrayContainerSorter<ValueType>(comparator),
            _pivotFactor(pivotFactor)
        {}

        void Sort(Container& container) override
        {
            if (container.IsEmpty())
                return;

            QSort(container, 0, container.GetCount() - 1);
        }

    private:
        float _pivotFactor;

        /// @brief Partitioning for quick sorting
        /// @param container The container
        /// @param low The low index
        /// @param high The high index
        /// @return The index of the pivot
        uint64 QSortPartition(Container& container, uint64 low, uint64 high) noexcept
        {
            using std::swap;

            // TODO: this can cause stack overflow if there are tons of items to sort
            uint64 pivotIndex = high;

            // Select the last element as the pivot
            ValueType* pivot = &container[pivotIndex];

            // Start at the lowest element
            uint64 j = low;

            // Swap all elements that should be in front of the pivot
            for (uint64 i = low; i <= high - 1; i++)
            {
                if (this->_comparator(container[i], *pivot))
                {
                    swap(container[i], container[j]);
                    j++;
                }
            }

            // Put the pivot into its position
            swap(container[j], container[high]);

            // All elements preceding j are less than the pivot, so the next pivot index will be after j
            return j;
        }

        /// @brief Sorts a portion of this list via quick sorting
        /// @param container The container
        /// @param low The low index
        /// @param high The high index
        void QSort(Container& container, uint64 low, uint64 high) noexcept
        {
            if (low >= high)
                return;

            // Partition and get the next pivot
            uint64 pi = QSortPartition(container, low, high);

            // Sort on either side of the next pivot
            if(pi > low)
                QSort(container, low, pi - 1);

            QSort(container, pi + 1, high);
        }
    };
} // Coco

#endif //COCOENGINE_QSORTER_H