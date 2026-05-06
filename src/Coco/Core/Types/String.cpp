//
// Created by cullen on 2/25/26.
//

#include "String.h"

#include <cstdarg>
#include <cstdio>

namespace Coco
{
    std::hash<std::string_view> _hasher;

    String FormatString(const char* format, ...)
    {
        constexpr int FormatBufferSize = 16384;

        char formattedMessage[FormatBufferSize]{};

        // Format the variable arguments into the message
        va_list args;
        va_start(args, format);
        int written = vsnprintf(formattedMessage, FormatBufferSize - 1, format, args);
        va_end(args);

        return String(std::basic_string_view<char>(formattedMessage, written));
    }

    uint64 ToHash(const String& str) noexcept
    {
        return _hasher(str.CStr());
    }

    uint64 ToHash(const char* str) noexcept
    {
        return _hasher(str);
    }

    bool operator==(const String& a, const String& b) noexcept
    {
        return strcmp(a.CStr(), b.CStr()) == 0;
    }

    bool operator==(const String& a, const char* b) noexcept
    {
        return strcmp(a.CStr(), b) == 0;
    }
}
