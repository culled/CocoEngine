//
// Created by cullen on 3/19/26.
//

#ifndef COCOENGINE_SLANGBLOB_H
#define COCOENGINE_SLANGBLOB_H
#include "slang.h"
#include "SlangType.h"
#include "Coco/Core/Types/Array.h"
#include "Coco/Core/Types/CoreTypes.h"

namespace Coco
{
    class SlangBlob : public SlangType<SlangBlob, ISlangBlob>
    {
    public:
        SlangBlob(uint64 size);

        //SlangResult queryInterface(const SlangUUID& uuid, void** outObject) override;
        //uint32_t addRef() override;
        //uint32_t release() override;
        const void* getBufferPointer() override { return _buffer.Data(); }
        size_t getBufferSize() override { return _buffer.GetCount(); }

        Array<uint8>& AsArray() { return _buffer; }

    private:
        Array<uint8> _buffer;
        //uint32_t _refCount;
    };
} // Coco

#endif //COCOENGINE_SLANGBLOB_H