//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_SLANGTYPE_H
#define COCOENGINE_SLANGTYPE_H

#include <Coco/Core/Types/CoreTypes.h>
#include "Coco/Core/Asserts.h"

#include <slang.h>

namespace Coco
{
    template<typename ClassType, typename BaseSlangType>
    class SlangType : public BaseSlangType
    {
    public:
        SlangType() : _refCount(0)
        {}

        virtual ~SlangType() = default;

        SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override
        {
            ++_refCount;
            *outObject = static_cast<ClassType*>(this);
            return SLANG_OK;
        }

        uint32_t addRef() override
        {
            ++_refCount;
            return _refCount;
        }

        uint32_t release() override
        {
            COCO_ASSERT(_refCount > 0, "RefCount was 0");
            --_refCount;
            if (_refCount == 0)
            {
                delete static_cast<ClassType*>(this);
            }

            return _refCount;
        }

    private:
        uint32 _refCount;
    };
} // Coco

#endif //COCOENGINE_SLANGTYPE_H