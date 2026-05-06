//
// Created by cullen on 3/21/26.
//

#include "OpenGLShaderBufferInterface.h"
#include "Coco/Rendering/Graphics/ShaderCursor.h"
#include "Resources/OpenGLBuffer.h"
#include <glad/glad.h>
#include <slang.h>

namespace Coco
{
    OpenGLShaderBufferInterface::OpenGLShaderBufferInterface(slang::TypeLayoutReflection* blockTypeLayout, Ref<OpenGLBuffer> buffer, uint64 bufferOffset) :
        ShaderBufferInterface(blockTypeLayout),
        _buffer(buffer),
        _bufferOffset(bufferOffset)
    {
    }

    void OpenGLShaderBufferInterface::Write(const ShaderElementLocation& location, const void* data, uint64 dataSize)
    {
        uint8* bufferPtr = static_cast<uint8*>(_buffer->GetMappedPtr()) + _bufferOffset;
        memcpy(bufferPtr + location.ByteOffset, data, dataSize);
    }

    void OpenGLShaderBufferInterface::Flush()
    {
        glFlushMappedNamedBufferRange(_buffer->GetHandle(), static_cast<GLintptr>(_bufferOffset), static_cast<GLsizeiptr>(_blockTypeLayout->getSize()));
    }

    void OpenGLShaderBufferInterface::Bind()
    {
        uint32 bindingIndex = _blockTypeLayout->getBindingRangeFirstDescriptorRangeIndex(0);
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, _buffer->GetHandle(), static_cast<GLintptr>(_bufferOffset), static_cast<GLsizeiptr>(_blockTypeLayout->getSize()));
    }
} // Coco