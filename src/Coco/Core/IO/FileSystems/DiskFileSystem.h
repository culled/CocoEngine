//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_DISKFILESYSTEM_H
#define COCOENGINE_DISKFILESYSTEM_H
#include "Coco/Core/IO/FileSystem.h"

namespace Coco
{
    class DiskFileSystem : public FileSystem
    {
    public:
        DiskFileSystem(const FilePath& assetsPath, const FilePath& cachePath);

        File Open(const FilePath& path, FileOpenFlags openFlags, bool cachePath) override;
        bool Exists(const FilePath& path, bool cachePath) const override;

    private:
        FilePath _assetsPath;
        FilePath _cachePath;

    private:
        FilePath GetFullPath(const FilePath& path, bool cachePath) const;
    };
} // Coco

#endif //COCOENGINE_DISKFILESYSTEM_H