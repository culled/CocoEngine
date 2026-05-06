//
// Created by cullen on 3/9/26.
//

#include "OpenGLImage.h"
#include <glad/glad.h>

#include "Coco/Rendering/RHI/OpenGL/OpenGLUtils.h"

namespace Coco
{
    OpenGLImage::OpenGLImage(OpenGLGraphicsPlatform* platform, uint64 id, const ImageDescription& desc) :
        Image(id),
        _platform(platform),
        _image(0)
    {
        this->_description = desc;

        GLenum internalFormat = OpenGLUtils::ToInternalFormat(desc.PixelFormat);
        glCreateTextures(GL_TEXTURE_2D, 1, &_image);
        glTextureStorage2D(_image, 1, internalFormat, static_cast<int>(desc.Width), static_cast<int>(desc.Height));

        //glTextureParameteri(_image, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTextureParameteri(_image, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTextureParameteri(_image, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        //glTextureParameteri(_image, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTextureParameteri(_image, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    OpenGLImage::~OpenGLImage()
    {
        if (_image)
        {
            glDeleteTextures(1, &_image);
        }
    }
} // Coco