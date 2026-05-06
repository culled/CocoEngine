//
// Created by cullen on 3/21/26.
//

#ifndef COCOENGINE_OPENGLUNIFORMSTORAGE_H
#define COCOENGINE_OPENGLUNIFORMSTORAGE_H

#include <Coco/Core/Types/CoreTypes.h>

#include "Coco/Rendering/Graphics/PagedLinearBuffer.h"
#include "OpenGLShaderBufferInterface.h"
#include "Resources/OpenGLBuffer.h"

namespace Coco
{
    class OpenGLUniformStorage
    {
    public:
        OpenGLUniformStorage(GraphicsPlatform* platform, uint64 pageSize);

        OpenGLShaderBufferInterface* Allocate(uint64 id, slang::TypeLayoutReflection* blockLayout);
        void Bind(uint64 id);
        void Clear();

    private:
        PagedLinearBuffer<OpenGLBuffer> _pagedBuffers;
        Map<uint64, OpenGLShaderBufferInterface> _interfaces;
    };
} // Coco

#endif //COCOENGINE_OPENGLUNIFORMSTORAGE_H