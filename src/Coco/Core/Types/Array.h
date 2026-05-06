//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_ARRAY_H
#define COCOENGINE_ARRAY_H
#include "ArrayContainer.h"
#include "../Memory/MemoryOverrides.h"

namespace Coco
{
    /// @brief A variable sized array of values
    /// @tparam ValueType The value type
    template<typename ValueType>
    class Array : public ArrayContainer<ValueType>
    {
    public:
        Array(Allocator* allocator = nullptr, uint64 initialCapacity = 0) :
            _allocator(allocator),
            _data(nullptr),
            _capacity(0)
        {
            Array::EnsureCapacity(initialCapacity, false);
        }

        Array(std::initializer_list<ValueType> list, Allocator* allocator = nullptr) :
            Array(allocator, list.size())
        {
            this->_count = list.size();

            if (list.size() > 0)
                ConstructArray(_data, Span<const ValueType>(list));
        }

        Array(uint64 count, const ValueType& defaultValue, Allocator* allocator = nullptr) :
            Array(allocator, count)
        {
            this->_count = count;

            if (count > 0)
                ConstructArray(_data, defaultValue, count);
        }

        Array(const Array& array) :
            Array(array._allocator, array._count)
        {
            this->_count = array._count;

            if (array._count > 0)
                ConstructArray(_data, static_cast<Span<const ValueType>>(array));
        }

        Array(Span<const ValueType> values, Allocator* allocator = nullptr) :
            Array(allocator, values.size())
        {
            this->_count = values.size();

            if (values.size() > 0)
                ConstructArray(_data, values);
        }

        Array(Array&& other) noexcept :
            Array()
        {
            swap(*this, other);
        }

        ~Array() noexcept override
        {
            this->Clear(true);

            COCO_ASSERT(!_data, "Data wasn't freed");
        }

        Array& operator=(Array rhs)
        {
            swap(*this, rhs);
            return *this;
        }

        Array& operator=(Array&& rhs) noexcept
        {
            swap(*this, rhs);
            return *this;
        }

        friend void swap(Array& a, Array& b) noexcept
        {
            using std::swap;
            swap(a._allocator, b._allocator);
            swap(a._count, b._count);
            swap(a._capacity, b._capacity);
            swap(a._data, b._data);
        }

        uint64 GetCapacity() const noexcept override { return _capacity; }
        ValueType* Data() noexcept override { return _data; }
        const ValueType* Data() const noexcept override { return _data; }

    protected:
        void EnsureCapacity(uint64 newCapacity, bool growthPadding) override
        {
            if (!_allocator)
                _allocator = Allocator::GetDefaultAllocator();

            COCO_ASSERT(_allocator, "Allocator was null");

            if (_capacity >= newCapacity)
                return;

            if (growthPadding)
                newCapacity = static_cast<uint64>(newCapacity * _growthPaddingMultiplier);

            auto newMemory = static_cast<ValueType*>(_allocator->Allocate(newCapacity * sizeof(ValueType)));

            if (_data)
            {
                // TODO: this may call a copy constructor instead of a move constructor and can cause leaks
                for (uint64 i = 0; i < this->_count; i++)
                    Construct(newMemory + i, std::move(*(_data + i)));

                _allocator->Free(_data, _capacity * sizeof(ValueType));
            }

            _data = newMemory;
            _capacity = newCapacity;
        }

        void FreeMemory() noexcept override
        {
            if (!_data)
                return;

            _allocator->Free(_data, _capacity * sizeof(ValueType));
            _data = nullptr;
            _capacity = 0;
        }

    private:
        static constexpr double _growthPaddingMultiplier = 1.5;

        Allocator* _allocator;
        ValueType* _data;
        uint64 _capacity;
    };
} // Coco

#endif //COCOENGINE_ARRAY_H