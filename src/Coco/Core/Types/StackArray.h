//
// Created by cullen on 2/24/26.
//

#ifndef COCOENGINE_STACKARRAY_H
#define COCOENGINE_STACKARRAY_H
#include "ArrayContainer.h"
#include "Span.h"
#include "Exception.h"

namespace Coco
{
    template <typename ValueType, int Capacity>
    class StackArray :
        public ArrayContainer<ValueType>
    {
    public:
        StackArray() noexcept :
            _data{}
        {}

        StackArray(std::initializer_list<ValueType> list) :
            StackArray()
        {
            this->_count = list.size();

            if (this->_count > 0)
                ConstructArray(StackArray::Data(), Span<const ValueType>(list));
        }

        StackArray(const StackArray& other) :
            StackArray()
        {
            this->_count = other._count;

            if (this->_count > 0)
                ConstructArray(StackArray::Data(), static_cast<Span<const ValueType>>(other));
        }

        StackArray(StackArray&& other) noexcept :
            StackArray()
        {
            swap(*this, other);
        }

        ~StackArray() noexcept override
        {
            this->DestructData();
        }

        friend void swap(StackArray& a, StackArray& b) noexcept
        {
            using std::swap;

            swap(a._data, b._data);
            swap(a._count, b._count);
        }

        ValueType* Data() noexcept override { return reinterpret_cast<ValueType*>(_data); }
        const ValueType* Data() const noexcept override { return reinterpret_cast<const ValueType*>(_data); }
        uint64 GetCapacity() const noexcept override { return Capacity; }

        StackArray& operator=(StackArray rhs)
        {
            swap(*this, rhs);
            return *this;
        }

        StackArray& operator=(StackArray&& rhs) noexcept
        {
            swap(*this, rhs);

            return *this;
        }

    protected:
        void EnsureCapacity(uint64 capacity, bool growthPadding) override
        {
            if (capacity > Capacity)
                throw OutOfRangeException("Desired capacity exceeds StackArray capacity");
        }

        void FreeMemory() noexcept override {}

    private:
        uint8 _data[Capacity * sizeof(ValueType)];
    };
} // Coco

#endif //COCOENGINE_STACKARRAY_H