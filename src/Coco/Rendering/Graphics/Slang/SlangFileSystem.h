//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_SLANGFILESYSTEM_H
#define COCOENGINE_SLANGFILESYSTEM_H
#include "slang.h"
#include "SlangType.h"
#include "Coco/Core/IO/FileSystem.h"

namespace Coco
{
    class SlangFileSystem : public SlangType<SlangFileSystem, ISlangFileSystem>
    {
    public:
        SlangFileSystem();

        void* castAs(const SlangUUID& guid) override { return this; }

        SlangResult loadFile(char const* path, ISlangBlob** outBlob) override;

    private:
        FileSystem* _fs;
    };
} // Coco

#endif //COCOENGINE_SLANGFILESYSTEM_H