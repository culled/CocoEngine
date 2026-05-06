//
// Created by cullen on 3/21/26.
//

#ifndef COCOENGINE_OPENGLSHADERBUFFERINTERFACE_H
#define COCOENGINE_OPENGLSHADERBUFFERINTERFACE_H
#include "Coco/Rendering/Graphics/ShaderBufferInterface.h"
#include <Coco/Core/Memory/Refs.h>

namespace Coco
{
    class OpenGLBuffer;

    class OpenGLShaderBufferInterface : public ShaderBufferInterface
    {
    public:
        OpenGLShaderBufferInterface(slang::TypeLayoutReflection* blockTypeLayout, Ref<OpenGLBuffer> buffer, uint64 bufferOffset);

        void Write(const ShaderElementLocation& location, const void* data, uint64 dataSize) override;
        void Flush() override;
        void Bind();

    private:
        Ref<OpenGLBuffer> _buffer;
        uint64 _bufferOffset;
    };
} // Coco

#endif //COCOENGINE_OPENGLSHADERBUFFERINTERFACE_H