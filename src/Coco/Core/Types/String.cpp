#include "String.h"

#include <regex>
#include <codecvt>

namespace Coco
{
    string TrimWhitespace(const string & str)
    {
        // https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string
        std::regex e("^\\s+|\\s+$"); // remove leading and trailing spaces
        return std::regex_replace(str, e, "");
    }

    string WideStringToString(const std::wstring& wideString)
    {
        using ConvertType = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<ConvertType, wchar_t> converter;

        return converter.to_bytes(wideString);
    }

    std::wstring StringToWideString(const string& string)
    {
        using ConvertType = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<ConvertType, wchar_t> converter;

        return converter.from_bytes(string);
    }
}
