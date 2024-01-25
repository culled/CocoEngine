#include "Corepch.h"
#include "FileTypes.h"

namespace Coco
{
    const char* IOStateToString(std::ios_base::iostate state)
    {
        switch (state)
        {
        case std::ios_base::goodbit:
            return "Good";
        case std::ios_base::badbit:
            return "Irrecoverable stream error";
        case std::ios_base::failbit:
            return "Stream operation failed";
        case std::ios_base::eofbit:
            return "End of file reached";
        default:
            return "Unknown IO state";
        }
    }

    string FileOpenFlagsToString(FileOpenFlags flags)
    {
        string result;

        if ((flags & FileOpenFlags::Read) == FileOpenFlags::Read)
        {
            result += "Read";
        }

        if ((flags & FileOpenFlags::Write) == FileOpenFlags::Write)
        {
            if (!result.empty())
                result += ", ";

            result += "Write";
        }

        if ((flags & FileOpenFlags::Write) == FileOpenFlags::Text)
        {
            if (!result.empty())
                result += ", ";

            result += "Text";
        }

        if (result.empty())
            return "None";

        return result;
    }
}