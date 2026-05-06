//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_STRINGCONTAINER_H
#define COCOENGINE_STRINGCONTAINER_H
#include "CoreTypes.h"
#include "Coco/Core/Asserts.h"

#include <cstring>
#include <locale>
#include <string_view>

namespace Coco
{
    namespace Detail
    {
        void ThrowOutOfRangeException(const char* messageFormat, uint64 index, uint64 size);
    }

    template<typename CharType>
    class StringContainer
    {
    public:
        virtual ~StringContainer() = default;

        virtual CharType* Data() noexcept = 0;
        virtual const CharType* Data() const noexcept = 0;
        virtual uint64 GetCapacity() const noexcept = 0;

        operator std::basic_string_view<CharType>() const noexcept { return std::basic_string_view<CharType>(Data(), _length); }

        CharType& operator[](uint64 index) noexcept { return Data()[index]; }
        CharType operator[](uint64 index) const noexcept { return Data()[index]; }

        const CharType* CStr() const noexcept { return Data(); }
        uint64 GetLength() const noexcept { return _length; }
        bool IsEmpty() const noexcept { return _length == 0;}

        CharType& At(uint64 index)
        {
            if (index >= _length)
                Detail::ThrowOutOfRangeException("Index is out of bounds: 0 <= %u < %u", index, _length);

            return Data()[index];
        }

        CharType At(uint64 index) const
        {
            if (index >= _length)
                Detail::ThrowOutOfRangeException("Index is out of bounds: 0 <= %u < %u", index, _length);

            return Data()[index];
        }

        void Append(const CharType* str)
        {
            if (!str)
                return;

            uint64 length = strlen(str);
            Append(str, length);
        }

        void Append(const StringContainer& str)
        {
            Append(str.Data(), str._length);
        }

        void Append(const CharType* str, uint64 additionalLength)
        {
            if (additionalLength == 0)
                return;

            // Ensure we can store the new total length and the null terminator
            uint64 totalLength = _length + additionalLength;
            EnsureCapacity(totalLength + 1, true);

            // Copy the additional string to the end of this one
            CharType* endPtr = Data() + _length;
            memcpy(endPtr, str, additionalLength * sizeof(CharType));

            _length = totalLength;
            EnsureNullTerminator();
        }

        void Set(const CharType* str)
        {
            if (!str)
            {
                Set(nullptr, 0);
                return;
            }

            uint64 length = strlen(str);
            Set(str, length);
        }

        void Set(const StringContainer& str)
        {
            Set(str.Data(), str._length);
        }

        void Set(const CharType* str, uint64 length)
        {
            // Ensure we can store the string and the null terminator
            EnsureCapacity(length + 1, false);
            _length = length;

            // Copy the contents into this string
            if(_length > 0)
                memcpy(Data(), str, length * sizeof(CharType));

            EnsureNullTerminator();
        }

        void Resize(uint64 newSize, bool preserveContents = true)
        {
            EnsureCapacity(newSize + 1, preserveContents);
            _length = newSize;
            EnsureNullTerminator();
        }

        /// @brief Attempts to find the given substring within this string
        /// @param substr The substring to find
        /// @param startOffset The offset to start looking for the substring in this string
        /// @param caseSensitive Whether to use case-sensitive matching
        /// @return The index of the first character of the substring in this string, or -1 if the substring wasn't found
        int64 Find(const std::basic_string_view<CharType>& substr, uint64 startOffset = 0, bool caseSensitive = true) const
        {
            if (startOffset >= _length)
                return -1;

            auto result = std::search(
                cbegin() + startOffset, cend(),
                substr.cbegin(), substr.cend(),
                [caseSensitive](CharType a, CharType b)
                {
                    if (caseSensitive)
                        return a == b;

                    return std::tolower(a) == std::tolower(b);
                });

            return result != cend() ? std::distance(cbegin(), result) : -1;
        }

        int64 Find(CharType c, uint64 startOffset = 0, bool caseSensitive = true) const
        {
            return Find(std::basic_string_view<CharType>(&c, 1), startOffset, caseSensitive);
        }

        int64 IndexOf(CharType searchChar, uint64 startOffset = 0) const noexcept
        {
            for (uint64 i = startOffset; i < _length; i++)
            {
                if (operator[](i) == searchChar)
                    return static_cast<int64>(i);
            }

            return -1;
        }

        int64 LastIndexOf(CharType searchChar, uint64 startOffset = 0) const noexcept
        {
            if (startOffset >= _length)
                return -1;

            for (int64 i = _length - (1 + startOffset); i >= 0; --i)
            {
                if (operator[](i) == searchChar)
                    return i;
            }

            return -1;
        }

        void Clear()
        {
            Resize(0, false);
        }

        bool StartsWith(const CharType* subStr, uint64 subStrLength) const
        {
            if (_length < subStrLength)
                return false;

            for (uint64 i = 0; i < subStrLength; i++)
            {
                if (operator[](i) != subStr[i])
                    return false;
            }

            return true;
        }

        bool StartWith(const CharType* subStr) const
        {
            uint64 subStrLength = strlen(subStr);
            return StartsWith(subStr, subStrLength);
        }

        bool StartsWith(CharType c) const
        {
            if (_length == 0)
                return false;

            return operator[](0) == c;
        }

        bool StartsWith(const StringContainer& str) const
        {
            return StartsWith(str.CStr(), str._length);
        }

        bool EndsWith(const CharType* subStr, uint64 subStrLength) const
        {
            if (_length < subStrLength)
                return false;

            uint64 offset = _length - subStrLength;

            for (uint64 i = 0; i < subStrLength; i++)
            {
                if (operator[](i + offset) != subStr[i])
                    return false;
            }

            return true;
        }

        bool EndsWith(const CharType* subStr) const
        {
            uint64 subStrLength = strlen(subStr);
            return EndsWith(subStr, subStrLength);
        }

        bool EndsWith(CharType c) const
        {
            if (_length == 0)
                return false;

            return operator[](_length - 1) == c;
        }

        bool EndsWith(const StringContainer& str) const
        {
            return EndsWith(str.CStr(), str._length);
        }

        bool Contains(const std::basic_string_view<CharType>& substr, bool caseSensitive = true) const
        {
            return Find(substr, 0, caseSensitive) != -1;
        }

        std::basic_string_view<CharType>::iterator begin() { return std::basic_string_view<CharType>(Data(), _length).begin(); }
        std::basic_string_view<CharType>::iterator end() { return std::basic_string_view<CharType>(Data(), _length).end(); }

        std::basic_string_view<CharType>::const_iterator begin() const { return std::basic_string_view<CharType>(Data(), _length).begin(); }
        std::basic_string_view<CharType>::const_iterator end() const { return std::basic_string_view<CharType>(Data(), _length).end(); }

        std::basic_string_view<CharType>::const_iterator cbegin() const { return std::basic_string_view<CharType>(Data(), _length).cbegin(); }
        std::basic_string_view<CharType>::const_iterator cend() const { return std::basic_string_view<CharType>(Data(), _length).cend(); }

    protected:
        uint64 _length = 0;

        virtual void EnsureCapacity(uint64 newCapacity, bool preserveData) = 0;

        void EnsureNullTerminator()
        {
            COCO_ASSERT(GetCapacity() >= _length, "Capacity was not larger than the string length");
            Data()[_length] = static_cast<CharType>('\0');
        }
    };
} // Coco

#endif //COCOENGINE_STRINGCONTAINER_H