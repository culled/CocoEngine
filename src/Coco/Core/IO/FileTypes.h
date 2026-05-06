//
// Created by cullen on 3/10/26.
//

#ifndef COCOENGINE_FILETYPES_H
#define COCOENGINE_FILETYPES_H
#include "../Types/CoreTypes.h"
#include "../Types/EnumTypes.h"
#include "Coco/Core/Types/Exception.h"
#include "Coco/Core/Types/String.h"

namespace Coco
{
    class FilePath;

    /// @brief Flags to open a file with
    enum class FileOpenFlags : uint8
    {
        None = 0,

        /// @brief The file will be opened with read permissions
        Read = 1 << 0,

        /// @brief The file will be opened with write permissions
        Write = 1 << 1
    };

    EnumFlagOperators(FileOpenFlags)

    /// @brief An exception thrown when a file cannot be opened
    class FileOpenException :
        public Exception
    {
    public:
        FileOpenException(const FilePath& filePath);
    };

    /// @brief An exception thrown when a file cannot be created
    class FileCreateException :
        public Exception
    {
    public:
        FileCreateException(const String& message);
    };

    /// @brief An exception thrown when a file operation fails
    class FileOperationException :
        public Exception
    {
    public:
        FileOperationException(const String& message);
    };
}
#endif //COCOENGINE_FILETYPES_H