//
// Created by cullen on 3/14/26.
//

#ifndef COCOENGINE_OPENGLBUFFER_H
#define COCOENGINE_OPENGLBUFFER_H
#include "Coco/Rendering/Graphics/Resources/Buffer.h"
#include "Coco/Rendering/Graphics/Resources/BufferTypes.h"
#include <glad/glad.h>

namespace Coco
{
    class OpenGLGraphicsPlatform;

    class OpenGLBuffer : public Buffer
    {
    public:
        OpenGLBuffer(OpenGLGraphicsPlatform* platform, uint64 id, uint64 size, BufferUsageFlags usageFlags);
        ~OpenGLBuffer();

        uint64 GetSize() const override { return _size; }
        void SetData(uint64 offset, Span<const uint8> data) override;
        void* GetMappedPtr() override { return _mappedPtr; }
        void Resize(uint64 newSize) override;

        uint32 GetHandle() const { return _buffer; }

    private:
        OpenGLGraphicsPlatform* _platform;
        uint64 _size;
        BufferUsageFlags _usageFlags;
        GLenum _glBufferUsage;
        uint32 _buffer;
        void* _mappedPtr;
    };
} // Coco

#endif //COCOENGINE_OPENGLBUFFER_H