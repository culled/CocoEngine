//
// Created by cullen on 3/12/26.
//
#include "FileTypes.h"

namespace Coco
{
    FileOpenException::FileOpenException(const FilePath& filePath) :
        Exception("Failed to open file") // TODO: better error
    {}

    FileCreateException::FileCreateException(const String& message) :
        Exception(message.CStr())
    {}

    FileOperationException::FileOperationException(const String& message) :
        Exception(message.CStr())
    {}
}
