//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_FILESYSTEM_H
#define COCOENGINE_FILESYSTEM_H
#include "File.h"

namespace Coco
{

    /// @brief A base class for a virtual FileSystem that the engine uses
    class FileSystem
    {
    public:
        virtual ~FileSystem() = default;

        /// @brief Opens a file at the given path
        /// @param path The path to the file
        /// @param openFlags Flags to open the file with
        /// @param cachePath If true, the file will be opened in the cache path
        /// @return The opened file
        virtual File Open(const FilePath& path, FileOpenFlags openFlags, bool cachePath) = 0;

        /// @brief Determines if a file exists at the given path
        /// @param path The path to the file
        /// @param cachePath If true, the file will be searched for in the cache path
        /// @return True if the file exists
        virtual bool Exists(const FilePath& path, bool cachePath) const = 0;
    };
} // Coco

#endif //COCOENGINE_FILESYSTEM_H