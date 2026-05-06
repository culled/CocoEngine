//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_OPENGLGRAPHICSSURFACE_H
#define COCOENGINE_OPENGLGRAPHICSSURFACE_H
#include "../../../Graphics/Resources/GraphicsSurface.h"
#include <EGL/egl.h>

#include "Coco/Core/Memory/Refs.h"

namespace Coco
{
    class OpenGLImage;
    class OpenGLGraphicsPlatform;

    class OpenGLGraphicsSurface : public GraphicsSurface
    {
    public:
        OpenGLGraphicsSurface(OpenGLGraphicsPlatform* _platform, uint64 id, EGLConfig config, EGLSurface surface, EGLContext context, const Sizei& framebufferSize);
        ~OpenGLGraphicsSurface();

        void SetFramebufferSize(const Sizei& framebufferSize) override;
        Sizei GetFramebufferSize() const override { return _framebufferSize; }

        EGLSurface GetSurface() const { return _surface; };
        Ref<OpenGLImage> GetNativeImage() { return _nativeImage; }

        bool MakeCurrent();
        void SwapBuffers();

    private:
        OpenGLGraphicsPlatform* _platform;
        EGLSurface _surface;
        EGLConfig _config;
        EGLContext _context;
        Sizei _framebufferSize;
        uint32 _fbo;
        Ref<OpenGLImage> _nativeImage;

    private:
        void RecreateNativeImage();
    };
} // Coco

#endif //COCOENGINE_OPENGLGRAPHICSSURFACE_H