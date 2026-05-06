//
// Created by cullen on 3/19/26.
//

#include "DiskFileSystem.h"

namespace Coco
{
    DiskFileSystem::DiskFileSystem(const FilePath& assetsPath, const FilePath& cachePath) :
        _assetsPath(assetsPath),
        _cachePath(cachePath)
    {}

    File DiskFileSystem::Open(const FilePath& path, FileOpenFlags openFlags, bool cachePath)
    {
        FilePath fullPath = GetFullPath(path, cachePath);
        return File(fullPath, openFlags);
    }

    bool DiskFileSystem::Exists(const FilePath& path, bool cachePath) const
    {
        FilePath fullPath = GetFullPath(path, cachePath);
        return File::Exists(fullPath);
    }

    FilePath DiskFileSystem::GetFullPath(const FilePath& path, bool cachePath) const
    {
        if (!path.IsRelative())
            return path;

        const FilePath& basePath = cachePath ? _cachePath : _assetsPath;
        return FilePath::Combine(basePath, path);
    }
} // Coco