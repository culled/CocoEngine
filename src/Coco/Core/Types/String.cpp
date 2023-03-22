#include "String.h"

#include <regex>

namespace Coco
{
    string TrimWhitespace(const string & str)
    {
        // https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string
        std::regex e("^\\s+|\\s+$"); // remove leading and trailing spaces
        return std::regex_replace(str, e, "");
    }
}
