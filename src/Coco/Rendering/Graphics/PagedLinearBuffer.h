//
// Created by cullen on 3/17/26.
//

#ifndef COCOENGINE_PAGEDLINEARBUFFER_H
#define COCOENGINE_PAGEDLINEARBUFFER_H
#include "GraphicsPlatform.h"
#include "Coco/Core/Memory/Refs.h"
#include "Coco/Core/Types/Array.h"
#include "Resources/BufferTypes.h"

namespace Coco
{
    template<typename BufferType>
    class PagedLinearBuffer
    {
    public:
        struct PagedBuffer
        {
            Ref<BufferType> TargetBuffer;
            uint64 RemainingBytes;
            uint64 LastAllocationFrameNumber;

            PagedBuffer(Ref<BufferType> targetBuffer, uint64 frameNumber) :
                TargetBuffer(targetBuffer),
                RemainingBytes(targetBuffer->GetSize()),
                LastAllocationFrameNumber(frameNumber)
            {}
        };

    public:
        PagedLinearBuffer(GraphicsPlatform* platform, const BufferDescription& bufferDescription, uint64 alignment) :
            _platform(platform),
            _alignment(alignment),
            _description(bufferDescription),
            _buffers()
        {}

        ~PagedLinearBuffer()
        {
            for (auto& buffer : _buffers)
                _platform->InvalidateResource(buffer.TargetBuffer->GetID());

            _buffers.Clear(true);
        }

        void Allocate(uint64 size, Ref<BufferType>& outBuffer, uint64& outBufferOffset)
        {
            COCO_ASSERT(size <= _description.Size, "Size is too big for a buffer");

            uint64 frameNumber = _platform->GetCurrentFrameNumber();

            for (auto& buffer : _buffers)
            {
                if (buffer.RemainingBytes >= size)
                {
                    outBuffer = buffer.TargetBuffer;
                    uint64 bufferSize = buffer.TargetBuffer->GetSize();
                    outBufferOffset = Math::GetAlignmentOffset(bufferSize - buffer.RemainingBytes, _alignment);
                    buffer.RemainingBytes = bufferSize - (outBufferOffset + size);
                    buffer.LastAllocationFrameNumber = frameNumber;
                    return;
                }
            }

            outBuffer = _platform->CreateBuffer(_description).Downcast<BufferType>();
            outBufferOffset = 0;

            auto& buffer = _buffers.EmplaceBack(outBuffer, frameNumber);
            buffer.RemainingBytes -= size;
        }

        void Clear()
        {
            for (auto& buffer : _buffers)
            {
                buffer.RemainingBytes = buffer.TargetBuffer->GetSize();
            }
        }

    private:
        GraphicsPlatform* _platform;
        uint64 _alignment;
        BufferDescription _description;
        Array<PagedBuffer> _buffers;
    };
} // Coco

#endif //COCOENGINE_PAGEDLINEARBUFFER_H