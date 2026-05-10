//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_STRING_H
#define COCOENGINE_STRING_H

#include "StringContainer.h"
#include "Coco/Core/Asserts.h"
#include "Coco/Core/Types/CoreTypes.h"
#include "Coco/Core/Memory/Allocator.h"

#include <cstring>
#include <string_view>

namespace Coco
{
    /// @brief Base class for a dynamic-length string of characters
    /// @tparam CharType The character type
    /// @tparam StackCapacity The maximum capacity until memory needs to be dynamically allocated
    template<typename CharType, int StackCapacity = 24>
    class BaseString :
        public StringContainer<CharType>
    {
    public:
        BaseString(Allocator* allocator = nullptr) noexcept :
            _allocator(allocator),
            _capacity(0),
            _data(nullptr),
            _stackData{}
        {}

        BaseString(const CharType* str, Allocator* allocator = nullptr) :
            BaseString(allocator)
        {
            StringContainer<CharType>::Set(str);
        }

        BaseString(const BaseString& other) :
            BaseString(other.CStr(), other._allocator)
        {}

        explicit BaseString(const std::basic_string_view<CharType> str, Allocator* allocator = nullptr) :
            BaseString(allocator)
        {
            StringContainer<CharType>::Set(str.data(), str.length());
        }

        BaseString(BaseString&& other) noexcept :
            BaseString()
        {
            swap(*this, other);
        }

        ~BaseString() noexcept override
        {
            if (_data)
            {
                COCO_ASSERT(_allocator, "Allocator was null");
                _allocator->Free(_data, _capacity * sizeof(CharType));
            }
        }

        BaseString& operator=(BaseString rhs)
        {
            swap(*this, rhs);
            return *this;
        }

        BaseString& operator=(BaseString&& rhs) noexcept
        {
            swap(*this, rhs);
            return *this;
        }

        BaseString& operator=(const CharType* rhs)
        {
            StringContainer<CharType>::Set(rhs);
            return *this;
        }

        BaseString& operator+=(const BaseString& rhs)
        {
            StringContainer<CharType>::Append(rhs);
            return *this;
        }

        BaseString& operator+=(const CharType* rhs)
        {
            StringContainer<CharType>::Append(rhs);
            return *this;
        }

        friend void swap(BaseString& a, BaseString& b) noexcept
        {
            using std::swap;
            swap(a._allocator, b._allocator);
            swap(a._length, b._length);
            swap(a._capacity, b._capacity);
            swap(a._data, b._data);
            swap(a._stackData, b._stackData);
        }

        CharType* Data() noexcept override { return _capacity > StackCapacity ? _data : &_stackData[0]; }
        const CharType* Data() const noexcept override { return _capacity > StackCapacity ? _data : &_stackData[0]; }
        uint64 GetCapacity() const noexcept override { return _capacity - 1; }

        /// @brief Ensures this string has the memory to hold the given number of characters
        /// @param capacity The minimum capacity to reserve
        void Reserve(uint64 capacity)
        {
            BaseString::EnsureCapacity(capacity + 1, true);
        }

        /// @brief Gets a part of this string and returns it
        /// @param offset The offset of the first character within this string
        /// @param length The length of the returned string, or -1 to get the remaining string
        /// @return The substring
        BaseString GetSubstring(uint64 offset, int64 length = -1) const
        {
            if (length < 0)
                length = this->GetLength() - offset;

            if (offset + length > this->GetLength())
                return BaseString();

            const CharType* ptr = Data() + offset;
            std::basic_string_view<CharType> view(ptr, length);

            return BaseString(view);
        }

    protected:
        void EnsureCapacity(uint64 newCapacity, bool preserveData) override
        {
            if (!_allocator)
                _allocator = Allocator::GetDefaultAllocator();

            COCO_ASSERT(_allocator, "No allocator was found");

            CharType* newMemory = nullptr;
            bool allocated = false;

            if (newCapacity <= StackCapacity)
            {
                // Our stack memory is large enough, so use that
                newMemory = _stackData;
            }
            else
            {
                // If we already have enough capacity in the heap, we don't need to do anything
                if (_capacity >= newCapacity)
                    return;

                // Create new memory for the contents
                newMemory = static_cast<CharType*>(_allocator->Allocate(newCapacity * sizeof(CharType)));
                allocated = true;
            }

            // Copy the current contents to the new memory if we should preserve the data and we're not going from stack data to stack data
            if(preserveData && (allocated || newMemory != this->Data()))
                memcpy(newMemory, this->Data(), this->_length * sizeof(CharType));

            // Free the old data if it exists, since we'll just use the new data
            if (_data)
            {
                _allocator->Free(_data, _capacity * sizeof(CharType));
                _data = nullptr;
            }

            if (allocated)
                _data = newMemory;

            _capacity = allocated ? newCapacity : StackCapacity;
        }
    private:
        Allocator* _allocator;
        uint64 _capacity;
        CharType* _data;
        CharType _stackData[StackCapacity];
    };

    template<typename CharType>
    BaseString<CharType> operator+(const BaseString<CharType>& lhs, const BaseString<CharType>& rhs)
    {
        BaseString ret(lhs);
        ret.Append(rhs);
        return ret;
    }

    template<typename CharType>
    BaseString<CharType> operator+(const BaseString<CharType>& lhs, const CharType* rhs)
    {
        BaseString ret(lhs);
        ret.Append(rhs);
        return ret;
    }

    /// @brief A dynamic-length string of characters
    using String = BaseString<char>;

    /// @brief A dynamic-length string of wide characters
    using WString = BaseString<wchar_t>;

    /// @brief Dynamically formats a string with the given values in sprintf format
    /// @param format The format string
    /// @param ... The values that will be inserted into the format string
    /// @return The formatted string
    String FormatString(const char* format, ...);

    /// @brief Computes a hash value for a string
    /// @param str The string
    /// @return The hashed value
    uint64 ToHash(const String& str) noexcept;

    /// @brief Computes a hash value for a string
    /// @param str The string
    /// @return The hashed value
    uint64 ToHash(const char* str) noexcept;

    bool operator==(const String& a, const String& b) noexcept;
    bool operator==(const String& a, const char* b) noexcept;
} // Coco

namespace std
{
    template<>
    struct hash<Coco::String>
    {
        size_t operator()(const Coco::String& str) const
        {
            return Coco::ToHash(str);
        }
    };
}
#endif //COCOENGINE_STRING_H