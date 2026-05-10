//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_DISKFILESYSTEM_H
#define COCOENGINE_DISKFILESYSTEM_H
#include "Coco/Core/IO/FileSystem.h"

namespace Coco
{
    /// @brief A FileSystem that accesses files within an assets and cache path in a file-folder directory structure
    class DiskFileSystem : public FileSystem
    {
    public:
        DiskFileSystem(const FilePath& assetsPath, const FilePath& cachePath);

        File Open(const FilePath& path, FileOpenFlags openFlags, bool cachePath) override;
        bool Exists(const FilePath& path, bool cachePath) const override;

    private:
        FilePath _assetsPath;
        FilePath _cachePath;

        /// @brief Converts a relative path into an absolute path
        /// @param path The path
        /// @param cachePath If true, the base path will be within the cache directory
        /// @return The absolute path
        FilePath GetFullPath(const FilePath& path, bool cachePath) const;
    };
} // Coco

#endif //COCOENGINE_DISKFILESYSTEM_H