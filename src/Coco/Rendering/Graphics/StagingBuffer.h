//
// Created by cullen on 3/28/26.
//

#ifndef COCOENGINE_STAGINGBUFFER_H
#define COCOENGINE_STAGINGBUFFER_H
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Rendering/RenderGraph/RenderGraphTypes.h"

namespace Coco
{
    class Buffer;

    struct StagingOperation
    {
        Ref<Buffer> StagingBuffer;
        uint64 BufferOffset;
        uint8* BufferPtr;
        uint64 Size;

        StagingOperation();
        StagingOperation(uint64 size);
        virtual ~StagingOperation() = default;
    };

    class StagingBuffer
    {
    public:
        virtual ~StagingBuffer() = default;

        virtual StagingOperation* CreateStagingOperation(uint64 size) = 0;
    };
} // Coco

#endif //COCOENGINE_STAGINGBUFFER_H