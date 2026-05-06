//
// Created by cullen on 3/4/26.
//

#ifndef COCOENGINE_X11OPENGLGRAPHICSSURFACEFACTORY_H
#define COCOENGINE_X11OPENGLGRAPHICSSURFACEFACTORY_H

#include "Coco/Core/Memory/Refs.h"

namespace Coco
{
    class X11Window;
    class OpenGLGraphicsPlatform;
    class GraphicsSurface;

    class X11OpenGLGraphicsSurfaceFactory
    {
    public:
        static Ref<GraphicsSurface> Create(OpenGLGraphicsPlatform& graphicsPlatform, const X11Window& window);
    };
} // Coco

#endif //COCOENGINE_X11OPENGLGRAPHICSSURFACEFACTORY_H