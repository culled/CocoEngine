//
// Created by cullen on 2/25/26.
//

#ifndef COCOENGINE_STRINGCONTAINER_H
#define COCOENGINE_STRINGCONTAINER_H
#include "CoreTypes.h"
#include "Coco/Core/Asserts.h"
#include "Coco/Core/Math/Math.h"

#include <cstring>
#include <locale>
#include <string_view>

namespace Coco
{
    namespace Detail
    {
        /// @brief Helper function for templates to throw an out-of-range exception
        /// @param messageFormat The message
        /// @param index The index value
        /// @param size The size
        void ThrowOutOfRangeException(const char* messageFormat, uint64 index, uint64 size);
    }

    /// @brief Base class for a string of characters
    /// @tparam CharType The character type
    template<typename CharType>
    class StringContainer
    {
    public:
        virtual ~StringContainer() = default;

        /// @brief Gets a pointer to the first character of the string
        /// @return The pointer to the first character
        virtual CharType* Data() noexcept = 0;

        /// @brief Gets a pointer to the first character of the string
        /// @return The pointer to the first character
        virtual const CharType* Data() const noexcept = 0;

        /// @brief Gets the maximum number of characters this string can hold
        /// @return The maximum number of characters this string can hold
        virtual uint64 GetCapacity() const noexcept = 0;

        operator std::basic_string_view<CharType>() const noexcept { return std::basic_string_view<CharType>(Data(), _length); }

        CharType& operator[](uint64 index) noexcept { return Data()[index]; }
        CharType operator[](uint64 index) const noexcept { return Data()[index]; }

        /// @brief Gets a null-terminated string
        /// @return A pointer to the first character
        const CharType* CStr() const noexcept { return Data(); }

        /// @brief Gets the number of characters in this string
        /// @return The number of characters in this string
        uint64 GetLength() const noexcept { return _length; }

        /// @brief Determines if this string is empty
        /// @return True if this string is empty
        bool IsEmpty() const noexcept { return _length == 0;}

        /// @brief Gets the character at the given position in this string
        /// @param index The character index
        /// @return The character at the position in this string
        CharType& At(uint64 index)
        {
            if (index >= _length)
                Detail::ThrowOutOfRangeException("Index is out of bounds: 0 <= %u < %u", index, _length);

            return Data()[index];
        }

        /// @brief Gets the character at the given position in this string
        /// @param index The character index
        /// @return The character at the position in this string
        CharType At(uint64 index) const
        {
            if (index >= _length)
                Detail::ThrowOutOfRangeException("Index is out of bounds: 0 <= %u < %u", index, _length);

            return Data()[index];
        }

        /// @brief Adds a string to the end of this string
        /// @param str The string
        void Append(const CharType* str)
        {
            if (!str)
                return;

            uint64 length = strlen(str);
            Append(str, length);
        }

        /// @brief Adds a string to the end of this string
        /// @param str The string
        void Append(const StringContainer& str)
        {
            Append(str.Data(), str._length);
        }

        /// @brief Adds part of a string to this string
        /// @param str The string
        /// @param maxLength The maximum number of characters in the given string to add
        void Append(const CharType* str, uint64 maxLength)
        {
            if (maxLength == 0)
                return;

            // Ensure the additional length doesn't extend outside the string's length
            maxLength = Math::Min(strlen(str), maxLength);

            // Ensure we can store the new total length and the null terminator
            uint64 totalLength = _length + maxLength;
            EnsureCapacity(totalLength + 1, true);

            // Copy the additional string to the end of this one
            CharType* endPtr = Data() + _length;
            memcpy(endPtr, str, maxLength * sizeof(CharType));

            _length = totalLength;
            EnsureNullTerminator();
        }

        /// @brief Sets this string to equal the given string
        /// @param str The string
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

        /// @brief Sets this string to equal the given string
        /// @param str The string
        void Set(const StringContainer& str)
        {
            Set(str.Data(), str._length);
        }

        /// @brief Sets this string to equal part of the given string
        /// @param str The string
        /// @param maxLength The maximum number of characters of the string to add
        void Set(const CharType* str, uint64 maxLength)
        {
            // Ensure the additional length doesn't extend outside the string's length
            maxLength = Math::Min(strlen(str), maxLength);

            // Ensure we can store the string and the null terminator
            EnsureCapacity(maxLength + 1, false);
            _length = maxLength;

            // Copy the contents into this string
            if(_length > 0)
                memcpy(Data(), str, maxLength * sizeof(CharType));

            EnsureNullTerminator();
        }

        /// @brief Sets the size of this string to the given length
        /// @param newLength The new length
        /// @param preserveContents If true, the contents of this string will be preserved if new memory is allocated to accommodate the difference in size
        void Resize(uint64 newLength, bool preserveContents = true)
        {
            EnsureCapacity(newLength + 1, preserveContents);
            _length = newLength;
            EnsureNullTerminator();
        }

        /// @brief Clears this string and sets its length to zero
        void Clear()
        {
            Resize(0, false);
        }

        /// @brief Attempts to find the given substring within this string
        /// @param substr The substring to find
        /// @param startOffset The offset to start looking for the substring in this string
        /// @param caseSensitive If true, case-sensitive matching will be used
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

        /// @brief Attempts to find the given substring within this string
        /// @param substr The substring to find
        /// @param startOffset The offset to start looking for the substring in this string
        /// @param caseSensitive If true, case-sensitive matching will be used
        /// @return The index of the first character of the substring in this string, or -1 if the substring wasn't found
        int64 Find(const CharType* substr, uint64 startOffset = 0, bool caseSensitive = true) const
        {
            return Find(std::basic_string_view<CharType>(substr), startOffset, caseSensitive);
        }

        /// @brief Determines if this string contains the given substring
        /// @param subStr The substring to find
        /// @param caseSensitive If true, case-sensitive matching will be used
        /// @return True if this string contains the substring
        bool Contains(const std::basic_string_view<CharType>& subStr, bool caseSensitive = true) const
        {
            return Find(subStr, 0, caseSensitive) != -1;
        }

        /// @brief Determines if this string contains the given substring
        /// @param subStr The substring to find
        /// @param caseSensitive If true, case-sensitive matching will be used
        /// @return True if this string contains the substring
        bool Contains(const CharType* subStr, bool caseSensitive = true) const
        {
            return Contains(std::basic_string_view<CharType>(subStr), caseSensitive);
        }

        /// @brief Attempts to find the first occurrence of the given character in this string
        /// @param searchChar The character to find
        /// @param startOffset The offset to start looking for the character in this string
        /// @return The index of the first occurrence of the character in this string, or -1 if it could not be found
        int64 IndexOf(CharType searchChar, uint64 startOffset = 0) const noexcept
        {
            for (uint64 i = startOffset; i < _length; i++)
            {
                if (operator[](i) == searchChar)
                    return static_cast<int64>(i);
            }

            return -1;
        }

        /// @brief Attempts to find the last occurrence of the given character in this string
        /// @param searchChar The character to find
        /// @param endOffset The offset from the end of this string to start looking for the character
        /// @return The index of the last occurrence of the character in this string, or -1 if it could not be found
        int64 LastIndexOf(CharType searchChar, uint64 endOffset = 0) const noexcept
        {
            if (endOffset >= _length)
                return -1;

            for (int64 i = _length - (1 + endOffset); i >= 0; --i)
            {
                if (operator[](i) == searchChar)
                    return i;
            }

            return -1;
        }

        /// @brief Determines if this string starts with the given substring
        /// @param str The substring to find
        /// @param caseSensitive If true, case-sensitive matching will be used
        /// @return True if this string starts with the substring
        bool StartsWith(const std::basic_string_view<CharType>& str, bool caseSensitive = true) const
        {
            return Find(str, 0, caseSensitive) == 0;
        }

        /// @brief Determines if this string starts with the given substring
        /// @param str The substring to find
        /// @param caseSensitive If true, case-sensitive matching will be used
        /// @return True if this string starts with the substring
        bool StartsWith(const CharType* str, bool caseSensitive = true) const
        {
            return StartsWith(std::basic_string_view<CharType>(str), caseSensitive);
        }

        /// @brief Determines if this string starts with the given character
        /// @param c The character
        /// @return True if this string starts with the character
        bool StartsWith(CharType c) const
        {
            if (_length == 0)
                return false;

            return operator[](0) == c;
        }

        /// @brief Determines if this string ends with the given substring
        /// @param str The substring to find
        /// @param caseSensitive If true, case-sensitive matching will be used
        /// @return True if this string ends with the substring
        bool EndsWith(const std::basic_string_view<CharType>& str, bool caseSensitive = true) const
        {
            if (str.size() > _length)
                return false;

            uint64 startOffset = _length - str.size();
            return Find(str, startOffset, caseSensitive) == startOffset;
        }

        /// @brief Determines if this string ends with the given substring
        /// @param str The substring to find
        /// @param caseSensitive If true, case-sensitive matching will be used
        /// @return True if this string ends with the substring
        bool EndsWith(const CharType* str, bool caseSensitive = true) const
        {
            return EndsWith(std::basic_string_view<CharType>(str), caseSensitive);
        }

        /// @brief Determines if this string ends with the given character
        /// @param c The character
        /// @return True if this string ends with the character
        bool EndsWith(CharType c) const
        {
            if (_length == 0)
                return false;

            return operator[](_length - 1) == c;
        }

        std::basic_string_view<CharType>::iterator begin() { return std::basic_string_view<CharType>(Data(), _length).begin(); }
        std::basic_string_view<CharType>::iterator end() { return std::basic_string_view<CharType>(Data(), _length).end(); }

        std::basic_string_view<CharType>::const_iterator begin() const { return std::basic_string_view<CharType>(Data(), _length).begin(); }
        std::basic_string_view<CharType>::const_iterator end() const { return std::basic_string_view<CharType>(Data(), _length).end(); }

        std::basic_string_view<CharType>::const_iterator cbegin() const { return std::basic_string_view<CharType>(Data(), _length).cbegin(); }
        std::basic_string_view<CharType>::const_iterator cend() const { return std::basic_string_view<CharType>(Data(), _length).cend(); }

    protected:
        uint64 _length = 0;

        /// @brief Ensures this string can hold the given number of characters
        /// @param newCapacity The new capacity, including the null-terminator
        /// @param preserveData If true, the contents will be preserved if new memory is allocated
        virtual void EnsureCapacity(uint64 newCapacity, bool preserveData) = 0;

        /// @brief Ensures this string is null-terminated
        void EnsureNullTerminator()
        {
            COCO_ASSERT(GetCapacity() >= _length, "Capacity was not larger than the string length");
            Data()[_length] = static_cast<CharType>('\0');
        }
    };
} // Coco

#endif //COCOENGINE_STRINGCONTAINER_H