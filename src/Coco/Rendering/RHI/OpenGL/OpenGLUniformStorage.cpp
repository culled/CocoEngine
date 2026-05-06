//
// Created by cullen on 3/21/26.
//

#include "OpenGLUniformStorage.h"

#include "slang.h"

namespace Coco
{
    OpenGLUniformStorage::OpenGLUniformStorage(GraphicsPlatform* platform, uint64 pageSize) :
        _pagedBuffers(platform, pageSize, BufferUsageFlags::HostVisible | BufferUsageFlags::Uniform),
        _interfaces()
    {}

    OpenGLShaderBufferInterface* OpenGLUniformStorage::Allocate(uint64 id, slang::TypeLayoutReflection* blockLayout)
    {
        if (_interfaces.Contains(id))
            return &_interfaces.Get(id);

        uint64 dataSize = blockLayout->getSize();
        Ref<OpenGLBuffer> targetBuffer;
        uint64 bufferOffset = 0;

        if (dataSize > 0)
        {
            _pagedBuffers.Allocate(dataSize, targetBuffer, bufferOffset);
        }

        auto& interface = _interfaces.Emplace(id, blockLayout, targetBuffer, bufferOffset);
        return &interface;
    }

    void OpenGLUniformStorage::Bind(uint64 id)
    {
        if (auto interface = _interfaces.TryGetValue(id))
        {
            interface->Bind();
        }
    }

    void OpenGLUniformStorage::Clear()
    {
    }
} // Coco