//
// Created by cullen on 3/19/26.
//

#include "SlangFileSystem.h"

#include "SlangBlob.h"
#include "Coco/Core/Engine.h"
#include "Coco/Core/Types/Array.h"

#include <slang-com-ptr.h>
#include <slang-cpp-types.h>

namespace Coco
{
    SlangFileSystem::SlangFileSystem() :
        _fs(nullptr)
    {
        _fs = Engine::Get()->GetFileSystem();
    }

    SlangResult SlangFileSystem::loadFile(char const* path, ISlangBlob** outBlob)
    {
        if (!_fs->Exists(path, false))
            return SLANG_E_NOT_FOUND;

        SlangBlob* blob = nullptr;

        try
        {
            File file = _fs->Open(path, FileOpenFlags::Read, false);

            blob = new SlangBlob(file.GetSize());
            file.ReadToEnd(blob->AsArray());
            file.Close();

            *outBlob = blob;
            return SLANG_OK;
        }
        catch (FileOpenException& ex)
        {
            COCO_ENGINE_LOG_ERROR("Failed to open shader module %s: %s", path, ex.what());

            if (blob)
                delete blob;

            return SLANG_E_CANNOT_OPEN;
        }
    }
} // Coco