//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_ARRAYCONTAINER_H
#define COCOENGINE_ARRAYCONTAINER_H
#include <functional>

#include "CoreTypes.h"
#include "Exception.h"
#include "Coco/Core/Memory/MemoryOverrides.h"

namespace Coco
{
    /// @brief Base class for arrays
    /// @tparam ValueType The value type
    template<typename ValueType>
    class ArrayContainer
    {
    public:
        /// @brief An iterator over an array
        class Iterator
        {
            friend class ArrayContainer;

        public:
            using value_type = ValueType;
            using pointer = ValueType*;
            using reference = ValueType&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

        public:
            Iterator(ArrayContainer& array, uint64 index) :
                _array(&array),
                _index(index)
            {}

            Iterator(const Iterator& other) :
                _array(other._array),
                _index(other._index)
            {}

            Iterator& operator=(Iterator rhs)
            {
                swap(*this, rhs);
                return *this;
            }

            friend void swap(Iterator& a, Iterator& b) noexcept
            {
                using std::swap;

                swap(a._array, b._array);
                swap(a._index, b._index);
            }

            pointer operator->()
            {
                return &(*_array)[_index];
            }

            pointer operator->() const
            {
                return &(*_array)[_index];
            }

            reference operator*()
            {
                return (*_array)[_index];
            }

            reference operator*() const
            {
                return (*_array)[_index];
            }

            Iterator& operator++() noexcept
            {
                ++_index;

                if (_index > _array->_count)
                    _index = _array->_count;

                return *this;
            }

            Iterator operator++(int)
            {
                Iterator copy(*this);
                ++*this;

                return copy;
            }

            Iterator& operator--() noexcept
            {
                if (_index == 0)
                    return *this;

                --_index;

                return *this;
            }

            Iterator operator--(int)
            {
                Iterator copy(*this);
                --*this;

                return copy;
            }

            bool operator==(const Iterator& other) const noexcept
            {
                return _array == other._array && _index == other._index;
            }

            bool operator!=(const Iterator& other) const noexcept
            {
                return !(*this == other);
            }

        private:
            ArrayContainer* _array;
            uint64 _index;
        };

        /// @brief A constant iterator over an array
        class ConstIterator
        {
            friend class ArrayContainer;

        public:
            using value_type = const ValueType;
            using pointer = const ValueType*;
            using reference = const ValueType&;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

        public:
            ConstIterator(const ArrayContainer& array, uint64 index) :
                _array(&array),
                _index(index)
            {}

            ConstIterator(const ConstIterator& other) :
                _array(other._array),
                _index(other._index)
            {}

            ConstIterator& operator=(ConstIterator rhs)
            {
                swap(*this, rhs);
                return *this;
            }

            pointer operator->() const
            {
                return &(*_array)[_index];
            }

            reference operator*() const
            {
                return (*_array)[_index];
            }

            friend void swap(ConstIterator& a, ConstIterator& b) noexcept
            {
                using std::swap;

                swap(a._array, b._array);
                swap(a._index, b._index);
            }

            ConstIterator& operator++() noexcept
            {
                ++_index;

                if (_index > _array->_count)
                    _index = _array->_count;

                return *this;
            }

            ConstIterator operator++(int) noexcept
            {
                ConstIterator copy(*this);
                ++*this;

                return copy;
            }

            ConstIterator& operator--() noexcept
            {
                if (_index == 0)
                    return *this;

                --_index;

                return *this;
            }

            ConstIterator operator--(int)
            {
                ConstIterator copy(*this);
                --*this;

                return copy;
            }

            bool operator==(const ConstIterator& other) const noexcept
            {
                return _array == other._array && _index == other._index;
            }

            bool operator!=(const ConstIterator& other) const noexcept
            {
                return !(*this == other);
            }

        private:
            const ArrayContainer* _array;
            uint64 _index;
        };

    public:
        ArrayContainer() noexcept :
            _count(0)
        {}

        virtual ~ArrayContainer() = default;

        /// @brief Gets the number of elements this array can hold before it needs to be resized
        /// @return The capacity
        virtual uint64 GetCapacity() const noexcept = 0;

        /// @brief Gets a pointer to the first element of this array
        /// @return A pointer to the first element
        virtual ValueType* Data() noexcept = 0;

        /// @brief Gets a pointer to the first element of this array
        /// @return A pointer to the first element
        virtual const ValueType* Data() const noexcept = 0;

        operator Span<ValueType>() { return Span<ValueType>(Data(), _count); }
        operator Span<const ValueType>() const { return Span<const ValueType>(Data(), _count); }

        ValueType& operator[](uint64 index) noexcept { return Data()[index]; }
        const ValueType& operator[](uint64 index) const noexcept { return Data()[index]; }

        /// @brief Adds an element to the end of this array
        /// @param value The value
        /// @return The added element
        ValueType& Append(const ValueType& value)
        {
            EnsureCapacity(_count + 1, true);

            ValueType* ptr = Data() + _count;
            Construct(ptr, value);
            ++_count;

            return *ptr;
        }

        /// @brief Adds an element to the end of this array
        /// @param value The value
        /// @return The added element
        ValueType& Append(ValueType&& value)
        {
            EnsureCapacity(_count + 1, true);

            ValueType* ptr = Data() + _count;
            Construct(ptr, std::move(value));
            ++_count;

            return *ptr;
        }

        ValueType* AppendRange(Span<const ValueType> values)
        {
            EnsureCapacity(_count + values.size(), true);

            ValueType* ptr = Data() + _count;
            if (!values.empty())
                ConstructArray(ptr, values);

            _count += values.size();

            return ptr;
        }

        /// @brief Constructs an element in-place at the back of this array
        /// @tparam Args The constructor args
        /// @param args Args to pass to the element's constructor
        /// @return The constructed element
        template<typename ... Args>
        ValueType& EmplaceBack(Args&& ... args)
        {
            EnsureCapacity(_count + 1, true);

            ValueType* ptr = Data() + _count;
            Construct(ptr, std::forward<Args>(args)...);
            ++_count;

            return *ptr;
        }

        /// @brief Gets the element at the specified index with bounds checking
        /// @param index The element index
        /// @return The element at the index
        ValueType& At(uint64 index)
        {
            if (index > _count)
                throw OutOfRangeException("Index is out of bounds");

            return Data()[index];
        }

        /// @brief Gets the element at the specified index with bounds checking
        /// @param index The element index
        /// @return The element at the index
        const ValueType& At(uint64 index) const
        {
            if (index > _count)
                throw OutOfRangeException("Index is out of bounds");

            return Data()[index];
        }

        /// @brief Gets the number of elements in this array
        /// @return The number of elements
        uint64 GetCount() const noexcept { return _count; }

        /// @brief Determines if this array is empty (has no elements)
        /// @return True if this array has no elements
        bool IsEmpty() const noexcept { return GetCount() == 0; }

        /// @brief Attempts to find the given element in this array
        /// @param value The value to find. Its equality operator will be used
        /// @return The index of the element in this array, or -1 if the value was not found
        int64 Find(const ValueType& value) const
        {
            for (uint64 i = 0; i < _count; ++i)
            {
                const ValueType* ptr = Data() + i;

                if (*ptr == value)
                    return static_cast<int64>(i);
            }

            return -1;
        }

        /// @brief Attempts to find the given element in this array using a predicate function
        /// @param predicate A function run for each element that should return "true" when the specified element is found
        /// @return The index of the element in this array, or -1 if the value was not found
        int64 Find(std::function<bool(const ValueType&)> predicate) const
        {
            COCO_ASSERT(predicate, "Predicate was null");

            for (uint64 i = 0; i < _count; ++i)
            {
                const ValueType* ptr = Data() + i;

                if (predicate(*ptr))
                    return static_cast<int64>(i);
            }

            return -1;
        }

        /// @brief Removes the element at the given index
        /// @param index The index of the element
        /// @param keepOrder If true, the order of the array will be maintained at the expense of shifting all elements after the removed element
        void RemoveAt(uint64 index, bool keepOrder = true) noexcept
        {
            using std::swap;

            if (index >= _count)
                return;

            //ValueType* ptr = Data() + index;

            if (keepOrder)
            {
                // Move all items up one position starting after the removed index
                /*for (uint64 i = index + 1; i < _count; i++)
                {
                    ValueType* source = Data() + i;
                    ptr = Data() + (i - 1);
                    *ptr = std::move(*source);
                }*/
                for (uint64 i = index; i < _count - 1; i++)
                {
                    ValueType* a = Data() + i;
                    ValueType* b = Data() + i + 1;
                    swap(*a, *b);
                }
            }
            else if (index < _count - 1)
            {
                // Move the last item to the one that was removed
                /*ValueType* source = Data() + (_count - 1);
                if (source != ptr)
                    *ptr = std::move(*source);*/
                ValueType* a = Data() + index;
                ValueType* b = Data() + (_count - 1);
                swap(*a, *b);
            }

            ValueType* ptr = Data() + (_count - 1);
            Destruct(ptr);

            --_count;
        }

        /// @brief Attempts to remove the given value from this array
        /// @param value The value to remove
        /// @param keepOrder If true, the order of the array will be maintained at the expense of shifting all elements after the removed element
        /// @return True if the value was found and removed
        bool Remove(const ValueType& value, bool keepOrder = true)
        {
            int64 index = Find(value);

            if (index == -1)
                return false;

            RemoveAt(index, keepOrder);
            return true;
        }

        bool RemoveIf(std::function<bool(const ValueType&)> predicate, bool keepOrder = true)
        {
            COCO_ASSERT(predicate, "Predicate was null");

            for (uint64 i = 0; i < _count; ++i)
            {
                const ValueType* ptr = Data() + i;

                if (predicate(*ptr))
                {
                    RemoveAt(i, keepOrder);
                    return true;
                }
            }

            return false;
        }

        /// @brief Gets the first element of this array
        /// @return The first element
        ValueType& Front()
        {
            if (IsEmpty())
                throw OutOfRangeException("Array is empty");

            return Data()[0];
        }

        /// @brief Gets the first element of this array
        /// @return The first element
        const ValueType& Front() const
        {
            if (IsEmpty())
                throw OutOfRangeException("Array is empty");

            return Data()[0];
        }

        /// @brief Gets the last element of this array
        /// @return The last element
        ValueType& Back()
        {
            if (IsEmpty())
                throw OutOfRangeException("Array is empty");

            return Data()[_count - 1];
        }

        /// @brief Gets the last element of this array
        /// @return The last element
        const ValueType& Back() const
        {
            if (IsEmpty())
                throw OutOfRangeException("Array is empty");

            return Data()[_count - 1];
        }

        /// @brief Clears this array and optionally frees its allocated memory
        /// @param freeMemory If true, the allocated memory of this array will be freed
        void Clear(bool freeMemory = false) noexcept
        {
            DestructData();

            if (freeMemory)
                FreeMemory();
        }

        /// @brief Reserves enough memory to ensure the array can hold the given number of elements without reallocating
        /// @param newCapacity The minimum capacity that the array should have
        void Reserve(uint64 newCapacity)
        {
            EnsureCapacity(newCapacity, false);
        }

        void Resize(uint64 count, const ValueType& insertItem = ValueType())
        {
            if (_count > count)
            {
                ValueType* ptr = Data() + count;
                DestructArray(ptr, _count - count);
            }
            else if (_count < count)
            {
                EnsureCapacity(count, false);
                ValueType* ptr = Data() + _count;
                ConstructArray(ptr, insertItem, count - _count);
            }

            _count = count;
        }

        bool Contains(const ValueType& value) const
        {
            return Find(value) != -1;
        }

        bool Contains(std::function<bool(const ValueType&)> predicate) const
        {
            return Find(predicate) != -1;
        }

        void Set(Span<const ValueType> values)
        {
            DestructData();

            EnsureCapacity(values.size(), false);
            ConstructArray(Data(), values);
            _count = values.size();
        }

        bool All(std::function<bool(const ValueType&)> predicate)
        {
            COCO_ASSERT(predicate, "Predicate was null");

            for (uint64 i = 0; i < _count; ++i)
            {
                const ValueType* ptr = Data() + i;

                if (!predicate(*ptr))
                {
                    return false;
                }
            }

            return true;
        }

        Iterator begin() { return Iterator(*this, 0); }
        Iterator end() { return Iterator(*this, _count); }

        ConstIterator begin() const { return ConstIterator(*this, 0); }
        ConstIterator end() const { return ConstIterator(*this, _count); }

    protected:
        uint64 _count;

        /// @brief Called to ensure this array can contain the given number of elements
        /// @param elementCount The number of elements this array needs to hold
        /// @param growthPadding If true, the array will allocate extra space to accomodate extra elements beyond the requested amount
        virtual void EnsureCapacity(uint64 elementCount, bool growthPadding) = 0;

        /// @brief Called to free any allocated memory by the array
        virtual void FreeMemory() noexcept = 0;

        /// @brief Destroys all elements in this array by calling their destructors
        void DestructData() noexcept
        {
            if (_count == 0)
                return;

            DestructArray(Data(), _count);
            _count = 0;
        }
    };
} // Coco

#endif //COCOENGINE_ARRAYCONTAINER_H