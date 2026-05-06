//
// Created by cullen on 3/22/26.
//

#ifndef COCOENGINE_RENDERINGENGINEPLATFORM_H
#define COCOENGINE_RENDERINGENGINEPLATFORM_H

namespace Coco
{
    class GraphicsPlatform;

    struct PlatformRenderingExtensions
    {
        virtual ~PlatformRenderingExtensions() = default;
    };

    class RenderingEnginePlatform
    {
    public:
        virtual ~RenderingEnginePlatform() = default;

        virtual void GetRenderingExtensions(GraphicsPlatform& graphicsPlatform, PlatformRenderingExtensions& extensions) = 0;
    };
} // Coco

#endif //COCOENGINE_RENDERINGENGINEPLATFORM_H