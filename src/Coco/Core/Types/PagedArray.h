//
// Created by cullen on 3/18/26.
//

#ifndef COCOENGINE_PAGEDARRAY_H
#define COCOENGINE_PAGEDARRAY_H

#include "Coco/Core/Types/Array.h"

namespace Coco
{
    /// @brief A set of arrays allocated in pages
    /// @tparam ValueType The array element type
    template<typename ValueType>
    class PagedArray
    {
    public:
        PagedArray(uint64 pageSize, Allocator* allocator = nullptr) noexcept :
            _allocator(allocator),
            _pageSize(pageSize),
            _pages(allocator)
        {}

        ~PagedArray() noexcept
        {
            _pages.Clear();
        }

        /// @brief Adds the given values to the paged array. NOTE: the number of elements must be no greater than the page size
        /// @param values The values to insert into the array
        /// @return The inserted values
        Span<ValueType> Allocate(Span<const ValueType> values)
        {
            for (auto& page : _pages)
            {
                uint64 originalSize = page.GetCount();
                if (page.GetCapacity() >= originalSize + values.size())
                {
                    ValueType* ptr = page.AppendRange(values);
                    return Span<ValueType>(ptr, values.size());
                }
            }

            auto& page = _pages.EmplaceBack(nullptr, _pageSize);
            ValueType* ptr = page.AppendRange(values);
            return Span<ValueType>(ptr, values.size());
        }

        /// @brief Allocates a number of elements into this array. NOTE: the number of elements must be no greater than the page size
        /// @param count The number of elements to add
        /// @return The inserted values
        Span<ValueType> Allocate(uint64 count)
        {
            for (auto& page : _pages)
            {
                uint64 originalSize = page.GetCount();
                if (page.GetCapacity() >= originalSize + count)
                {
                    page.Resize(originalSize + count);
                    ValueType* ptr = page.Data() + originalSize;
                    return Span<ValueType>(ptr, count);
                }
            }

            auto& page = _pages.EmplaceBack(_allocator, _pageSize);
            page.Resize(count);
            ValueType* ptr = page.Data();
            return Span<ValueType>(ptr, count);
        }

        /// @brief Adds the given values to the paged array. NOTE: the number of elements must be no greater than the page size
        /// @param values The values to add
        /// @return The emplaced values
        Span<ValueType> Emplace(Span<const ValueType> values)
        {
            auto result = Allocate(values.size());
            for (uint64 i = 0; i < values.size(); i++)
                result[i] = values[i];

            return result;
        }

        /// @brief Clears all values in the array
        void Clear() noexcept
        {
            for (auto& page : _pages)
                page.Clear();
        }

    private:
        Allocator* _allocator;
        uint64 _pageSize;
        Array<Array<ValueType>> _pages;
    };
} // Coco

#endif //COCOENGINE_PAGEDARRAY_H