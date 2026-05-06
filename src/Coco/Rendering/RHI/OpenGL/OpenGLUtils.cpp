//
// Created by cullen on 3/9/26.
//

#include "OpenGLUtils.h"

#include "Coco/Rendering/Graphics/Resources/Image.h"

namespace Coco
{
    GLenum OpenGLUtils::PixelFormatToAttachment(ImagePixelFormat format)
    {
        bool isDepth = Image::IsDepthFormat(format);
        bool isStencil = Image::IsStencilFormat(format);

        GLenum type;
        if (isDepth)
        {
            if (isStencil)
                return GL_DEPTH_STENCIL_ATTACHMENT;

            return GL_DEPTH_ATTACHMENT;
        }

        if (isStencil)
        {
            return GL_STENCIL_ATTACHMENT;
        }

        return GL_COLOR_ATTACHMENT0;
    }

    GLenum OpenGLUtils::ToInternalFormat(ImagePixelFormat format)
    {
        switch (format)
        {
            case ImagePixelFormat::RGBA8:
                return GL_RGBA8;
            case ImagePixelFormat::R32_Int:
                return GL_R32I;
            case ImagePixelFormat::R32G32_Int:
                return GL_RG32I;
            case ImagePixelFormat::R32G32B32_Int:
                return GL_RGB32I;
            case ImagePixelFormat::R32G32B32A32_Int:
                return GL_RGBA32I;
            case ImagePixelFormat::R32_UInt:
                return GL_R32UI;
            case ImagePixelFormat::R32G32_UInt:
                return GL_RG32UI;
            case ImagePixelFormat::R32G32B32_UInt:
                return GL_RGB32UI;
            case ImagePixelFormat::R32G32B32A32_UInt:
                return GL_RGBA32UI;
            case ImagePixelFormat::Depth24_Stencil8:
                return GL_DEPTH24_STENCIL8;
            default:
                return GL_NONE;
        }
    }

    GLenum OpenGLUtils::ToShaderStage(ShaderStageType stage)
    {
        switch (stage)
        {
            case ShaderStageType::Vertex:
                return GL_VERTEX_SHADER;
            case  ShaderStageType::Fragment:
                return GL_FRAGMENT_SHADER;
            case ShaderStageType::Geometry:
                return GL_GEOMETRY_SHADER;
            case  ShaderStageType::Compute:
                return GL_COMPUTE_SHADER;
            case ShaderStageType::Tesselation:
                return GL_TESS_CONTROL_SHADER;
            default:
                return GL_NONE;
        }
    }

    GLenum OpenGLUtils::ToBufferType(BufferUsageFlags usageFlags)
    {
        if ((usageFlags & BufferUsageFlags::Uniform) == BufferUsageFlags::Uniform)
            return GL_UNIFORM_BUFFER;

        if ((usageFlags & BufferUsageFlags::Vertex) == BufferUsageFlags::Vertex)
            return GL_ARRAY_BUFFER;

        if ((usageFlags & BufferUsageFlags::Index) == BufferUsageFlags::Index)
            return GL_ELEMENT_ARRAY_BUFFER;

        COCO_ASSERT(false, "Unknown buffer type");
        return GL_NONE;
    }

    GLenum OpenGLUtils::ToBufferUsage(BufferUsageFlags usageFlags)
    {
        GLenum result = 0;

        if ((usageFlags & BufferUsageFlags::HostVisible) == BufferUsageFlags::HostVisible)
            result |= GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

        return result;
    }
} // Coco