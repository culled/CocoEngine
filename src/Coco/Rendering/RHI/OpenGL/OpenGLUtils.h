//
// Created by cullen on 3/9/26.
//

#ifndef COCOENGINE_OPENGLUTILS_H
#define COCOENGINE_OPENGLUTILS_H
#include "Coco/Rendering/Graphics/Resources/ImageTypes.h"

#include "../../ShaderTypes.h"
#include "Coco/Rendering/Graphics/Resources/BufferTypes.h"
#include <glad/glad.h>

namespace Coco
{
    class OpenGLUtils
    {
    public:
        static GLenum PixelFormatToAttachment(ImagePixelFormat format);
        static GLenum ToInternalFormat(ImagePixelFormat format);
        static GLenum ToShaderStage(ShaderStageType stage);
        static GLenum ToBufferType(BufferUsageFlags usageFlags);
        static GLenum ToBufferUsage(BufferUsageFlags usageFlags);
    };
} // Coco

#endif //COCOENGINE_OPENGLUTILS_H