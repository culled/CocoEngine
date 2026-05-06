//
// Created by cullen on 3/12/26.
//

#ifndef COCOENGINE_BUFFER_H
#define COCOENGINE_BUFFER_H
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Rendering/Graphics/GraphicsResource.h"
#include "Coco/Rendering/Graphics/StagingBuffer.h"

namespace Coco
{
    class Buffer : public GraphicsResource
    {
    public:
        virtual ~Buffer() = default;

        virtual uint64 GetSize() const = 0;
        virtual void SetData(uint64 offset, Span<const uint8> data) = 0;
        virtual void* GetMappedPtr() = 0;
        virtual void Resize(uint64 newSize) = 0;
        virtual void CopyFrom(StagingOperation& stagingOperation) = 0;

    protected:
        Buffer(uint64 id);
    };

    struct BufferAllocation
    {
        Ref<Buffer> DataBuffer;
        uint64 BufferOffset;
        uint64 Size;

        BufferAllocation(Ref<Buffer> buffer, uint64 offset, uint64 size);
    };
} // Coco

#endif //COCOENGINE_BUFFER_H