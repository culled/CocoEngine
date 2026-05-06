//
// Created by cullen on 3/19/26.
//

#include "SlangBlob.h"

namespace Coco
{
    SlangBlob::SlangBlob(uint64 size) :
        _buffer(nullptr, size)
    {
        _buffer.Resize(size);
    }

    /*SlangResult SlangBlob::queryInterface(const SlangUUID& uuid, void** outObject)
    {
        ++_refCount;
        *outObject = static_cast<ISlangBlob*>(this);
        return SLANG_OK;
    }

    uint32_t SlangBlob::addRef()
    {
        ++_refCount;
    }

    uint32_t SlangBlob::release()
    {
        COCO_ASSERT(_refCount > 0, "RefCount was 0");
        --_refCount;
        if (_refCount == 0)
        {
            delete this;
        }

        return _refCount;
    }*/

    // void* SlangBlob::getInterface(const SlangUUID& uuid)
    // {
    //     if (uuid == ISlangUnknown::getTypeGuid() || uuid == ISlangBlob::getTypeGuid())
    //     {
    //         return static_cast<ISlangBlob*>(this);
    //     }
    //     return nullptr;
    // }
} // Coco