//
// Created by cullen on 3/4/26.
//

#include "OpenGLGraphicsSurface.h"
#include "../OpenGLGraphicsPlatform.h"
#include "OpenGLImage.h"
#include "Coco/Core/Engine.h"

#include <glad/glad.h>

namespace Coco
{
    OpenGLGraphicsSurface::OpenGLGraphicsSurface(OpenGLGraphicsPlatform* _platform, uint64 id, EGLConfig config, EGLSurface surface, EGLContext context, const Sizei& framebufferSize) :
        GraphicsSurface(id),
        _platform(_platform),
        _config(config),
        _surface(surface),
        _context(context),
        _framebufferSize(framebufferSize),
        _nativeImage(),
        _fbo(0)
    {
        // HACK: since OpenGL won't fully initialize without a context, do the initialization here now that we have one
        MakeCurrent();
        _platform->EnsureInitialized();

        RecreateNativeImage();

        glCreateFramebuffers(1, &_fbo);

        COCO_ENGINE_LOG_VERBOSE("Created OpenGLGraphicsSurface with size (%ux%u)", framebufferSize.Width, framebufferSize.Height);
    }

    OpenGLGraphicsSurface::~OpenGLGraphicsSurface()
    {
        if (_fbo)
            glDeleteFramebuffers(1, &_fbo);

        if (_context)
            eglDestroyContext(_platform->GetEGL(), _context);

        if (_surface)
            eglDestroySurface(_platform->GetEGL(), _surface);

        COCO_ENGINE_LOG_VERBOSE("Destroyed OpenGLGraphicsSurface");
    }

    void OpenGLGraphicsSurface::SetFramebufferSize(const Sizei& framebufferSize)
    {
        _framebufferSize = framebufferSize;

        RecreateNativeImage();

        COCO_ENGINE_LOG_VERBOSE("Resized OpenGLGraphicsSurface framebuffer to (%ux%u)", framebufferSize.Width, framebufferSize.Height);
    }

    bool OpenGLGraphicsSurface::MakeCurrent()
    {
        if (!eglMakeCurrent(_platform->GetEGL(), _surface, _surface, _context))
        {
            COCO_ENGINE_LOG_ERROR("Error while making context current: %d", eglGetError());
            return false;
        }

        return true;
    }

    void OpenGLGraphicsSurface::SwapBuffers()
    {
        // Blit the native image to the actual backbuffer
        glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _nativeImage->GetHandle(), 0);
        glBlitNamedFramebuffer(_fbo, 0,
            0, 0, _framebufferSize.Width, _framebufferSize.Height,
            0, 0, _framebufferSize.Width, _framebufferSize.Height,
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST);

        if (!eglSwapBuffers(_platform->GetEGL(), _surface))
        {
            COCO_ENGINE_LOG_ERROR("Error while swapping buffers: %d", eglGetError());
        }
    }

    void OpenGLGraphicsSurface::RecreateNativeImage()
    {
        if (_nativeImage)
            _platform->InvalidateResource(_nativeImage->GetID());

        // TODO: make description based on iamge config
        ImageDescription desc = ImageDescription::Create2D(_framebufferSize.Width, _framebufferSize.Height, ImagePixelFormat::RGBA8, ImageColorSpace::sRGB, ImageUsageFlags::RenderTarget | ImageUsageFlags::Sampled);
        _nativeImage = _platform->CreateImage(desc).Downcast<OpenGLImage>();
    }
} // Coco