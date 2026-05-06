//
// Created by cullen on 3/14/26.
//

#include "OpenGLBuffer.h"
#include "../OpenGLGraphicsPlatform.h"

#include <glad/glad.h>

#include "Coco/Rendering/RHI/OpenGL/OpenGLUtils.h"

namespace Coco
{
    OpenGLBuffer::OpenGLBuffer(OpenGLGraphicsPlatform* platform, uint64 id, uint64 size, BufferUsageFlags usageFlags) :
        Buffer(id),
        _platform(platform),
        _size(size),
        _usageFlags(usageFlags),
        _buffer(0),
        _glBufferUsage(OpenGLUtils::ToBufferUsage(usageFlags))
    {
        glCreateBuffers(1, &_buffer);
        glNamedBufferStorage(_buffer, static_cast<GLsizeiptr>(_size), nullptr, _glBufferUsage);
        //glNamedBufferData(_buffer, static_cast<GLsizeiptr>(size), nullptr, _bufferUpdateMode);

        if ((_glBufferUsage & GL_MAP_PERSISTENT_BIT) == GL_MAP_PERSISTENT_BIT)
        {
            _mappedPtr = glMapNamedBufferRange(_buffer, 0, static_cast<GLsizeiptr>(_size), _glBufferUsage);
            COCO_ASSERT(_mappedPtr, "Unable to map buffer");
        }
    }

    OpenGLBuffer::~OpenGLBuffer()
    {
        if (_buffer)
            glDeleteBuffers(1, &_buffer);
    }

    void OpenGLBuffer::SetData(uint64 offset, Span<const uint8> data)
    {
        //glNamedBufferSubData(_buffer, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), data.data());
        void* dst = glMapNamedBufferRange(_buffer, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(data.size()), GL_WRITE_ONLY);
        memcpy(dst, data.data(), data.size());
        glUnmapNamedBuffer(_buffer);
    }

    void OpenGLBuffer::Resize(uint64 newSize)
    {
        if (_buffer)
            glDeleteBuffers(1, &_buffer);

        glCreateBuffers(1, &_buffer);
        glNamedBufferData(_buffer, static_cast<GLsizeiptr>(newSize), nullptr, _glBufferUsage);
        _size = newSize;

        if ((_glBufferUsage & GL_MAP_PERSISTENT_BIT) == GL_MAP_PERSISTENT_BIT)
            _mappedPtr = glMapNamedBuffer(_buffer, GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
    }
} // Coco