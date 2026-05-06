//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_FILESYSTEM_H
#define COCOENGINE_FILESYSTEM_H
#include "File.h"

namespace Coco
{
    class FileSystem
    {
    public:
        virtual ~FileSystem() = default;

        virtual File Open(const FilePath& path, FileOpenFlags openFlags, bool cachePath) = 0;
        virtual bool Exists(const FilePath& path, bool cachePath) const = 0;
    };
} // Coco

#endif //COCOENGINE_FILESYSTEM_H