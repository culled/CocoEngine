//
// Created by cullen on 3/4/26.
//

#include "X11OpenGLGraphicsSurfaceFactory.h"
#include <Coco/Rendering/RHI/OpenGL/OpenGLGraphicsPlatform.h>
#include "../../../../../Rendering/RHI/OpenGL/Resources/OpenGLGraphicsSurface.h"
#include "../X11Window.h"

#include <EGL/egl.h>

namespace Coco
{
    Ref<GraphicsSurface> X11OpenGLGraphicsSurfaceFactory::Create(OpenGLGraphicsPlatform& graphicsPlatform,
        const X11Window& window)
    {
        EGLint attr[] = {
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,
            EGL_NONE
        };

        EGLConfig config;
        EGLint configCount;

        if (!eglChooseConfig(graphicsPlatform.GetEGL(), attr, &config, 1, &configCount))
        {
            String err = FormatString("Failed to choose config. EGL error: %d", eglGetError());
            throw Exception(err.CStr());
        }

        EGLSurface surface = eglCreateWindowSurface(graphicsPlatform.GetEGL(), config, window.GetNativeWindow(), nullptr);
        if (surface == EGL_NO_SURFACE)
        {
            String err = FormatString("Failed to create window surface. EGL error: %d", eglGetError());
            throw Exception(err.CStr());
        }

        EGLint ctxAttr[] = {
            EGL_CONTEXT_MAJOR_VERSION, 4,
            EGL_CONTEXT_MINOR_VERSION, 6,
            EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
            EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE, // TODO: select debug context
            EGL_NONE
        };

        EGLContext context = eglCreateContext(graphicsPlatform.GetEGL(), config, EGL_NO_CONTEXT, ctxAttr);
        if (context == EGL_NO_CONTEXT)
        {
            String err = FormatString("Failed to create EGL context: code %d", eglGetError());
            throw Exception(err.CStr());
        }

        return graphicsPlatform.CreateSurface(config, surface, context, window.GetSize());
    }
} // Coco